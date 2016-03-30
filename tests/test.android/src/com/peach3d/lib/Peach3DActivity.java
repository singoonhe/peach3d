package com.peach3d.lib;

import java.util.HashMap;
import java.util.Vector;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Color;
import android.graphics.Paint.FontMetricsInt;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.Bundle;
import android.util.Log;
import android.net.Uri;

public class Peach3DActivity extends android.app.NativeActivity {
    protected static Peach3DActivity mGlobalActivity;
    protected static Handler mGlobalHandler;

	@Override
    public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
        // system load library
        try {
            ActivityInfo app = getPackageManager().getActivityInfo(this.getComponentName(), PackageManager.GET_ACTIVITIES|PackageManager.GET_META_DATA);
            Bundle bundle = app.metaData;
            String libName = bundle.getString("android.app.lib_name");
            System.loadLibrary(libName);
        } catch (Exception e) {
            Log.e("Peach3D", "Can't find library from AndroidManifest.xml under <Activity>!");
        }

        // save activity hander
        mGlobalActivity = this;
        mGlobalHandler = new Handler();
    }

	/* The values are the same as Peach3D/Core/Peach3DTypes.h. */
	private static final int TEXTHALIGNMENT_LEFT = 0;
    private static final int TEXTHALIGNMENT_CENTER = 1;
	private static final int TEXTHALIGNMENT_RIGHT = 2;
	private static final int TEXTVALIGNMENT_TOP = 0;
    private static final int TEXTVALIGNMENT_CENTER = 1;
	private static final int TEXTVALIGNMENT_BOTTOM = 2;

    private static HashMap<String, String> mClickedStrMap = new HashMap<String, String>();
    // defined text draw base info
    private static class tTextDrawInfo {
    	PointF     	start;  // draw start pos
    	PointF	    size;   // draw text size
        int		    color;  // draw color
        String      str;    // draw text
        Bitmap      image;  // draw image
        tTextDrawInfo(PointF _start, PointF _size, Bitmap _image) {
            start = _start; size = _size; image = _image; str=null;
        }
        tTextDrawInfo(PointF _start, PointF _size, int _color, String _str) {
            start = _start; size = _size; color = _color; str= _str; image=null;
        }
    };

    // native function, create bitmap
    private static native Bitmap nativeCreateBitmapFromFile(final String file);
    // native function, add a click rect to label
    private static native void nativeAddLabelClickRect(final String key, int startX, int startY, int width, int height);

    private static PointF calcTextRenderInfoList(final String[] stringList, final String[] imageList,
        final int[] colorList, final boolean[] clickEnabledList, final Paint paint,
        final int imageVAlignment, final int pWidth, final int pHeight,
        final int screenWidth, Vector<Vector<tTextDrawInfo>> drawInfoList)
	{
        mClickedStrMap.clear();
        // calc space render size
        final FontMetricsInt fm = paint.getFontMetricsInt();
		final int spaceHeight = (int) Math.ceil(fm.descent - fm.ascent);
		PointF rSize = new PointF(pWidth, pHeight);
        if (pWidth == 0) {
            rSize.x = screenWidth;
        }

        float curWidth=0.0f, curHeight=0.0f;
        float lineMaxHeight=0.0f, textMaxWidth=0.0f;
        Vector<tTextDrawInfo> lineInfoList = new Vector<tTextDrawInfo>();
        for (int i=0; i<stringList.length; ++i) {
            float curDrawHeight = 0.0f;
            if (imageList[i].length() > 0) {
                Bitmap bmp = Peach3DActivity.nativeCreateBitmapFromFile(imageList[i]);
                if (bmp != null) {
                    float imageWidth = bmp.getWidth(), imageHeight = bmp.getHeight();
                    if (curWidth > 0.0f && (rSize.x-curWidth) < imageWidth) {
                        // direct change to newline
                        curHeight += lineMaxHeight;
                        if (curWidth > textMaxWidth) {
                            textMaxWidth = curWidth;
                        }
                        curWidth = 0.0f;
                        lineMaxHeight = 0.0f;
                        if (lineInfoList.size() > 0) {
                            drawInfoList.add(new Vector<tTextDrawInfo>(lineInfoList));
                        }
                        lineInfoList.clear();
                    }
                    // add image to list
                    float drawPosY = curHeight;
                    if (imageHeight < lineMaxHeight) {
                        if (imageVAlignment == TEXTVALIGNMENT_CENTER){
                            drawPosY += (lineMaxHeight - imageHeight)/2.0f;
                        }
                        else if (imageVAlignment == TEXTVALIGNMENT_BOTTOM) {
                            drawPosY += lineMaxHeight - imageHeight;
                        }
                    }
                    lineInfoList.add(new tTextDrawInfo(new PointF(curWidth, drawPosY), new PointF(imageWidth, imageHeight), bmp));
                    curWidth += imageWidth;
                    // save current draw height
                    curDrawHeight = imageHeight;
                }
            }
            else if (stringList[i].compareTo("\n")==0) {
                // direct change to newline
                curHeight += spaceHeight;
                if (curWidth > textMaxWidth) {
                    textMaxWidth = curWidth;
                }
                curWidth = 0.0f;
                lineMaxHeight = 0.0f;
                if (lineInfoList.size() > 0) {
                    drawInfoList.add(new Vector<tTextDrawInfo>(lineInfoList));
                }
                lineInfoList.clear();
            }
            else {
                String textStr = stringList[i];
                int lastLength = textStr.length();
                // calc if need change line
                while (lastLength>0 && textStr.length()>0) {
                	int drawLength = lastLength;
                	String drawStr = new String(textStr);
                    float drawWidth = paint.measureText(drawStr);
                    // calc last width could render string length
                    float lastDrawWidth = rSize.x-curWidth;
                    while (drawLength>0 && drawWidth >= lastDrawWidth) {
                        drawLength = drawLength - 1;
                        drawStr = drawStr.substring(0, drawLength);
                        drawWidth = paint.measureText(drawStr);
                    }
                    // calc text draw need pos y
                    float drawPosY = curHeight;
                    if (spaceHeight < lineMaxHeight) {
                        if (imageVAlignment == TEXTVALIGNMENT_CENTER){
                            drawPosY += (lineMaxHeight - spaceHeight)/2.0f;
                        }
                        else if (imageVAlignment == TEXTVALIGNMENT_BOTTOM) {
                            drawPosY += lineMaxHeight - spaceHeight;
                        }
                    }
                    lineInfoList.add(new tTextDrawInfo(new PointF(curWidth, drawPosY), new PointF(drawWidth, spaceHeight), colorList[i], drawStr));
                    curWidth += drawWidth;
                    // store click enabled string
                    if (clickEnabledList[i]) {
                    	mClickedStrMap.put(drawStr, stringList[i]);
                    }
                    if (lineMaxHeight == 0){
                        lineMaxHeight = spaceHeight;
                    }
                    if (drawLength < lastLength) {
                        // direct change to newline
                        curHeight += lineMaxHeight;
                        if (curWidth > textMaxWidth) {
                            textMaxWidth = curWidth;
                        }
                        curWidth = 0.0f;
                        lineMaxHeight = 0.0f;
                        if (lineInfoList.size() > 0) {
                            drawInfoList.add(new Vector<tTextDrawInfo>(lineInfoList));
                        }
                        lineInfoList.clear();
                        // last str
                        textStr = textStr.substring(drawLength);
                    }
                    lastLength -= drawLength;
                    // save current draw height
                    curDrawHeight = spaceHeight;
                }
            }

            if (curDrawHeight > lineMaxHeight) {
                // modify all previous tTextDrawInfo
            	for(tTextDrawInfo info : lineInfoList) {
                    if (curDrawHeight > info.size.y){
                        if (imageVAlignment == TEXTVALIGNMENT_CENTER){
                            info.start.y += (curDrawHeight - lineMaxHeight)/2.0f;
                        }
                        else if (imageVAlignment == TEXTVALIGNMENT_BOTTOM) {
                            info.start.y += curDrawHeight - lineMaxHeight;
                        }
                    }
                }
                lineMaxHeight = curDrawHeight;
            }
        }
        // add last render content
        if (lineInfoList.size() > 0) {
            curHeight += lineMaxHeight;
            drawInfoList.add(new Vector<tTextDrawInfo>(lineInfoList));
        }
        else {
            curHeight += spaceHeight;
        }
        if (curWidth > textMaxWidth) {
            textMaxWidth = curWidth;
        }

        // save text need render size
        if (pWidth == 0) {
            rSize.x = textMaxWidth;
        }
        rSize.y = curHeight;
        return rSize;
	}

	public static Bitmap createTextBitmap(final String[] stringList, final String[] imageList,
        final int[] colorList, final boolean[] clickEnabledList, final String pFontName,
        final int pFontSize, final int pAlignment, final int pWidth, final int pHeight, final int screenWidth)
    {
        // get text alignment and label image alignment
        final int hAlignment = pAlignment & 0x0F;
        final int vAlignment = (pAlignment >> 4) & 0x0F;
        final int imageVAlignment = (pAlignment >> 8) & 0x0F;
        // create paint
        final Paint paint = new Paint();
        paint.setTextSize(pFontSize);
        paint.setAntiAlias(true);
        paint.setTypeface(Typeface.create(pFontName, Typeface.NORMAL));

        // calc text need size
        final Vector<Vector<tTextDrawInfo>> drawInfoList = new Vector<Vector<tTextDrawInfo>>();
        PointF renderSize = Peach3DActivity.calcTextRenderInfoList(stringList, imageList, colorList, clickEnabledList, paint, imageVAlignment, pWidth, pHeight, screenWidth, drawInfoList);
        if (renderSize.x < 0.0001 || renderSize.y < 0.0001) {
            return null;
        }

        // apply horizontal alignment
        if (hAlignment != TEXTHALIGNMENT_LEFT) {
        	for(Vector<tTextDrawInfo> lineList : drawInfoList) {
                tTextDrawInfo lastInfo = lineList.get(lineList.size() - 1);
                // default for horizontal right
                float offsetX = renderSize.x - (lastInfo.start.x + lastInfo.size.x);
                if (hAlignment == TEXTHALIGNMENT_CENTER) {
                    // set horizontal center
                    offsetX = offsetX / 2.0f;
                }
            	for(tTextDrawInfo info : lineList) {
                    info.start.x += offsetX;
                };
            };
        }
        float startPosY = 0.0f;
        // apply vertical alignment
        if (pWidth > 0 && pHeight > 0) {
            if (vAlignment == TEXTVALIGNMENT_BOTTOM) {
                startPosY = pHeight - renderSize.y;
            }
            else if (vAlignment == TEXTVALIGNMENT_CENTER) {
                startPosY = (pHeight - renderSize.y)/2.0f;
            }
            renderSize.y = pHeight;
        }

        // create bitmap and canvas
        final FontMetricsInt fm = paint.getFontMetricsInt();
        final Bitmap bitmap = Bitmap.createBitmap((int)renderSize.x, (int)renderSize.y,
            Bitmap.Config.ARGB_8888);
        final Canvas canvas = new Canvas(bitmap);
        // make bitmap upside
        canvas.scale(1, -1);
        // just for test
        // paint.setColor(Color.WHITE);
        // canvas.drawRect(0, 0, (int)renderSize.x, (int)renderSize.y, paint);
        // draw text to bitmap
    	for(Vector<tTextDrawInfo> lineList : drawInfoList) {
        	for(tTextDrawInfo info : lineList) {
                if (info.start.y >= (-info.size.y) && info.start.y <= renderSize.y) {
                    if (info.image!=null && info.image.getWidth() > 0) {
                        info.start.y = info.start.y + startPosY;
                    	canvas.drawBitmap(info.image, info.start.x, info.start.y, paint);
                    }
                    else if (info.str!=null && info.str.length() > 0) {
                        info.start.y = info.start.y + startPosY + (info.size.y - fm.descent) - renderSize.y;
                    	paint.setColor(info.color);
                    	canvas.drawText(info.str, info.start.x, info.start.y, paint);

                        // add click enabled rect
                    	String findValue = mClickedStrMap.get(info.str);
                        if (findValue!=null && findValue.length() > 0) {
                        	Peach3DActivity.nativeAddLabelClickRect(findValue, (int)info.start.x, (int)info.start.y, (int)info.size.x, (int)info.size.y);
                        }
                    }
                }
            }
        }

        // return bitmap
        return bitmap;
    }

    public static void openUrl(final String url)
    {
        mGlobalHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    Intent intent = new Intent(Intent.ACTION_VIEW);
                    intent.setData(Uri.parse(url));
                    mGlobalActivity.startActivity(intent);
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }
}
