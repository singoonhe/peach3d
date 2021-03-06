﻿<?xml version="1.0" encoding="UTF-8"?>
<!-- Layout name will pass to LoadOverFunc. -->
<Layout name="TestLayout">
    <!-- If you want bind widget, just give it a name. -->
    <Widget>
        <Anchor>0, 0</Anchor>
        <!-- Set widget display color. -->
        <Color>0, 0, 1</Color>
        <!-- Set widget alpha. -->
        <Alpha>0.2</Alpha>
        <!-- SwallowEvents is used for shield events with below UI. -->
        <SwallowEvents>true</SwallowEvents>
        <Sprite name="LogoSprite">
            <Texture>peach3d_icon.png</Texture>
            <!-- Using screen center as orign position. -->
            <BindCorner>Center</BindCorner>
            <!-- Using min value between (screen size / design size) width and height. -->
            <AutoSize>Min, Min</AutoSize>
            <Scale>0.2, 0.2</Scale>
            <!-- Using min scale to zoom "Position". -->
            <AutoPosition>Min, Min</AutoPosition>
            <Position>0, 150</Position>
        </Sprite>
        <Label name="NoticeLabel">
            <Text>Logo show normal!</Text>
            <FontSize>30</FontSize>
            <!-- Using screen center as orign position. -->
            <BindCorner>Center</BindCorner>
        </Label>
        <Button name="ChangeButton" frame="peach3d_ui.xml">
            <NormalTexture>#common_normal.png</NormalTexture>
            <Text>Gray</Text>
            <FontSize>26</FontSize>
            <!-- FillColor just used for text color, Color for mix with texture. -->
            <FillColor>0, 1, 0</FillColor>
            <!-- Using screen center as orign position. -->
            <BindCorner>Center</BindCorner>
            <!-- Using min scale to zoom "Position". -->
            <AutoPosition>Min, Min</AutoPosition>
            <Position>0, -150</Position>
        </Button>
    </Widget>
</Layout>
