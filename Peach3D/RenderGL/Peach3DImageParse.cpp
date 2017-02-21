//
//  Peach3DImageParse.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/26/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DImageParse.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DResourceManager.h"
#include "libjpeg/jpeglib.h"
#include "libpng/png.h"
#if PEACH3D_CURRENT_RENDER != PEACH3D_RENDER_DX11
#include <setjmp.h>
#endif

namespace Peach3D
{
    /**************************** parse jpeg file data ***************************/
    struct PdJpegErrorMgr
    {
        struct jpeg_error_mgr pub;	/* "public" fields */
        jmp_buf setjmp_buffer;      /* for return to caller */
    };
    
    void pd_jpeg_error_exit (j_common_ptr cinfo)
    {
        /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
        PdJpegErrorMgr* pderr = (PdJpegErrorMgr*) cinfo->err;
        /* Always display the message. */
        /* We could postpone this until after returning, if we chose. */
        char buffer[1024];
        (*cinfo->err->format_message) (cinfo, buffer);
        Peach3DLog(LogLevel::eError, "jpeg error: %s", buffer);
        /* Return control to the setjmp point */
        longjmp(pderr->setjmp_buffer, 1);
    }
    
    void* jpegImageDataParse(const ResourceLoaderInput& input)
    {
        /* these are standard libjpeg structures for reading(decompression) */
        jpeg_decompress_struct cinfo;
        /* libjpeg data structure for storing one row, that is, scanline of an image */
        JSAMPROW row_pointer = nullptr;
        
        TextureLoaderRes* outData = new TextureLoaderRes();
        struct PdJpegErrorMgr jerr;
        do {
            /* We set up the normal JPEG error routines, then override error_exit. */
            cinfo.err = jpeg_std_error(&jerr.pub);
            jerr.pub.error_exit = pd_jpeg_error_exit;
            
            /* If we get here, the JPEG code has signaled an error.
             * We need to clean up the JPEG object, close the input file, and return.
             */
            IF_BREAK(setjmp(jerr.setjmp_buffer), nullptr)
            
            /* setup decompression process and source, then read JPEG header */
            jpeg_create_decompress( &cinfo );
            /* Step specify data source */
            jpeg_mem_src( &cinfo, (unsigned char *)input.data, input.length );
            /* reading the image header which contains image information */
            jpeg_read_header( &cinfo, TRUE );
            
            // we only support RGB or grayscale
            if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
                outData->format = TextureFormat::eI8;
            }
            else {
                cinfo.out_color_space = JCS_RGB;
                outData->format = TextureFormat::eRGB8;
            }
            IF_BREAK(outData->format == TextureFormat::eUnknow, "Unsupport jpeg format!!");
            
            /* Start decompression jpeg here */
            jpeg_start_decompress( &cinfo );
            
            /* init image info */
            outData->width  = cinfo.output_width;
            outData->height = cinfo.output_height;

            // malloc buffer to receive data
            uint widthByte = (outData->width) * (cinfo.output_components);
            // glTexImage2d requires rows to be 4-byte aligned
            widthByte += 3 - ((widthByte-1) % 4);
            // malloc return buffer
            row_pointer = static_cast<JSAMPROW>(malloc(sizeof(unsigned char) * widthByte));
            outData->size = widthByte * cinfo.output_height;
            unsigned long location = outData->size - widthByte;
            outData->buffer = static_cast<unsigned char*>(malloc((outData->size) * sizeof(unsigned char)));
            
            /* now actually read the jpeg into the raw buffer */
            /* read one scan line at a time */
            JSAMPROW cachePointer[1] = {row_pointer};
            while( cinfo.output_scanline < cinfo.output_height ) {
                jpeg_read_scanlines( &cinfo, cachePointer, 1 );
                memcpy(outData->buffer + location, &row_pointer[0], cinfo.output_width*cinfo.output_components);
                location -= widthByte;
            }
        } while (0);
        
        /* When read image file with broken data, jpeg_finish_decompress() may cause error.
         * Besides, jpeg_destroy_decompress() shall deallocate and release all memory associated
         * with the decompression object.
         * So it doesn't need to call jpeg_finish_decompress().
         */
        //jpeg_finish_decompress( &cinfo );
        jpeg_destroy_decompress(&cinfo);
        if (row_pointer) {
            free(row_pointer);
        }
        return outData;
    }
    
    /**************************** parse png file data ***************************/
    typedef struct
    {
        const unsigned char * data;
        size_t size;
        int offset;
    }tImageSource;
    
    static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        tImageSource* isource = (tImageSource*)png_get_io_ptr(png_ptr);
        
        if((int)(isource->offset + length) <= isource->size) {
            memcpy(data, isource->data+isource->offset, length);
            isource->offset += length;
        }
        else {
            Peach3DLog(LogLevel::eError, "pngReaderCallback failed");
        }
    }
    
    void* pngImageDataParse(const ResourceLoaderInput& input)
    {
        // length of bytes to check if it is a valid png file
#define PNGSIGSIZE  8
        png_structp     png_ptr     =   0;
        png_infop       info_ptr    = 0;
        
        TextureLoaderRes* outData = new TextureLoaderRes();
        do {
            // init png_struct
            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
            IF_BREAK(!png_ptr, nullptr)
            
            // init png_info
            info_ptr = png_create_info_struct(png_ptr);
            IF_BREAK(!info_ptr, nullptr)
            // set jump use for error
            setjmp(png_jmpbuf(png_ptr));
            
            // set the read call back function
            tImageSource imageSource;
            imageSource.data    = (unsigned char*)input.data;
            imageSource.size    = input.length;
            imageSource.offset  = 0;
            png_set_read_fn(png_ptr, &imageSource, pngReadCallback);
            
            // read png header info
            
            // read png file info
            png_read_info(png_ptr, info_ptr);
            
            outData->width = png_get_image_width(png_ptr, info_ptr);
            outData->height = png_get_image_height(png_ptr, info_ptr);
            png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
            png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);
            
            // force palette images to be expanded to 24-bit RGB
            // it may include alpha channel
            if (color_type == PNG_COLOR_TYPE_PALETTE) {
                png_set_palette_to_rgb(png_ptr);
            }
            // low-bit-depth grayscale images are to be expanded to 8 bits
            if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
                bit_depth = 8;
                png_set_expand_gray_1_2_4_to_8(png_ptr);
            }
            // expand any tRNS chunk data into a full alpha channel
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
                png_set_tRNS_to_alpha(png_ptr);
            }
           
            // Expanded earlier for grayscale, now take care of palette and rgb
            if (bit_depth < 8) {
                png_set_packing(png_ptr);
            }
            // reduce images with 16-bit samples to 8 bits
            if (bit_depth == 16) {
                png_set_strip_16(png_ptr);
            }
            // update info
            png_read_update_info(png_ptr, info_ptr);
            
            int pixelByte = 1;
            switch (color_type) {
                case PNG_COLOR_TYPE_GRAY:
                    outData->format = TextureFormat::eI8;
                    break;
                case PNG_COLOR_TYPE_GRAY_ALPHA:
                    outData->format = TextureFormat::eA8I8;
                    pixelByte = 2;
                    break;
                case PNG_COLOR_TYPE_RGB:
                case PNG_COLOR_TYPE_PALETTE:
                    outData->format = TextureFormat::eRGB8;
                    pixelByte = 3;
                    break;
                case PNG_COLOR_TYPE_RGB_ALPHA:
                    outData->format = TextureFormat::eRGBA8;
                    pixelByte = 4;
                    break;
                default:
                    break;
            }
            IF_BREAK(outData->format == TextureFormat::eUnknow, "Unsupport png format!!");
            
            uint widthByte = (outData->width) * (pixelByte);
            // glTexImage2d requires rows to be 4-byte aligned
            widthByte += 3 - ((widthByte-1) % 4);
            outData->size = widthByte * (outData->height);
            // read png data
            png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * (outData->height));
            outData->buffer = static_cast<unsigned char*>(malloc((outData->size) * sizeof(unsigned char)));
            for(int y = 0; y < outData->height; y++) {
                row_pointers[y] = outData->buffer + widthByte * (outData->height - y - 1);
            }
            png_read_image(png_ptr, row_pointers);
            
            // release cache data
            free(row_pointers);
            png_read_end(png_ptr, nullptr);
        } while (0);
        
        if (png_ptr) {
            png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
        }
        return outData;
    }
}
