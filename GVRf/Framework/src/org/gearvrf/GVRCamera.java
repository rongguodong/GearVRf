/* Copyright 2015 Samsung Electronics Co., LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.gearvrf;

import org.gearvrf.utility.Colors;

import android.content.res.Resources;
import android.graphics.Color;

/**
 * A GVR camera 'takes a picture' of the scene graph: there are typically two of
 * them, one for each eye. You can {@linkplain #setRenderMask(int) specify which
 * eye(s)} this camera is serving; {@linkplain #setBackgroundColor(int) set a
 * background color}; and add any number of
 * {@linkplain #addPostEffect(GVRPostEffect) 'post effects'} which are basically
 * (stock or custom) GL shaders, applied after the scene graph has been
 * rendered.
 */
public abstract class GVRCamera extends GVRComponent {
    protected GVRCamera(GVRContext gvrContext, long ptr) {
        super(gvrContext, ptr);
    }

    /** Get the background color as an Android {@link Color} */
    public int getBackgroundColor() {
        return Color.argb(Colors.glToByte(getBackgroundColorA()), //
                Colors.glToByte(getBackgroundColorR()), //
                Colors.glToByte(getBackgroundColorG()), //
                Colors.glToByte(getBackgroundColorB()));
    }

    /**
     * Set the background color.
     * 
     * If you don't set the background color, the default is an opaque black:
     * {@link Color#BLACK}, 0xff000000.
     * 
     * @param color
     *            An Android 32-bit (ARGB) {@link Color}, such as you get from
     *            {@link Resources#getColor(int)}
     */
    public void setBackgroundColor(int color) {
        setBackgroundColorR(Colors.byteToGl(Color.red(color)));
        setBackgroundColorG(Colors.byteToGl(Color.green(color)));
        setBackgroundColorB(Colors.byteToGl(Color.blue(color)));
        setBackgroundColorA(Colors.byteToGl(Color.alpha(color)));
    }

    /**
     * Sets the background color of the scene rendered by this camera.
     * 
     * If you don't set the background color, the default is an opaque black.
     * Meaningful parameter values are from 0 to 1, inclusive: values
     * {@literal < 0} are clamped to 0; values {@literal > 1} are clamped to 1.
     */
    public void setBackgroundColor(float r, float g, float b, float a) {
        setBackgroundColorR(r);
        setBackgroundColorG(g);
        setBackgroundColorB(b);
        setBackgroundColorA(a);
    }

    /**
     * @return Red component of the background color. Value ranges from 0 to 1,
     *         inclusive.
     */
    public float getBackgroundColorR() {
        return NativeCamera.getBackgroundColorR(getNative());
    }

    /**
     * Set the red component of the background color.
     * 
     * @param r
     *            Red component of the background color. Meaningful values are
     *            from 0 to 1, inclusive: values {@literal < 0} are clamped to
     *            0; values {@literal > 1} are clamped to 1.
     */
    public void setBackgroundColorR(float r) {
        NativeCamera.setBackgroundColorR(getNative(), r);
    }

    /**
     * @return Green component of the background color. Value ranges from 0 to
     *         1, inclusive.
     */
    public float getBackgroundColorG() {
        return NativeCamera.getBackgroundColorG(getNative());
    }

    /**
     * Set the green component of the background color.
     * 
     * @param g
     *            Green component of the background color. Meaningful values are
     *            from 0 to 1, inclusive: values {@literal < 0} are clamped to
     *            0; values {@literal > 1} are clamped to 1.
     */
    public void setBackgroundColorG(float g) {
        NativeCamera.setBackgroundColorG(getNative(), g);
    }

    /**
     * @return Blue component of the background color. Value ranges from 0 to 1,
     *         inclusive.
     */
    public float getBackgroundColorB() {
        return NativeCamera.getBackgroundColorB(getNative());
    }

    /**
     * Set the blue component of the background color.
     * 
     * @param b
     *            Blue component of the background color. Meaningful values are
     *            from 0 to 1, inclusive: values {@literal < 0} are clamped to
     *            0; values {@literal > 1} are clamped to 1.
     */
    public void setBackgroundColorB(float b) {
        NativeCamera.setBackgroundColorB(getNative(), b);
    }

    /**
     * @return Alpha component of the background color. Value ranges from 0 to
     *         1, inclusive.
     */
    public float getBackgroundColorA() {
        return NativeCamera.getBackgroundColorA(getNative());
    }

    /**
     * Set the alpha component of the background color.
     * 
     * @param a
     *            Value of alpha component.
     */
    public void setBackgroundColorA(float a) {
        NativeCamera.setBackgroundColorA(getNative(), a);
    }

    /**
     * Get the current render mask
     * 
     * @return A mask consisting of values from
     *         {@link GVRRenderData.GVRRenderMaskBit GVRRenderMaskBit}.
     */
    public int getRenderMask() {
        return NativeCamera.getRenderMask(getNative());
    }

    /**
     * Set the current render mask
     * 
     * @param renderMask
     *            A mask consisting of values from
     *            {@link GVRRenderData.GVRRenderMaskBit GVRRenderMaskBit}.
     */
    public void setRenderMask(int renderMask) {
        NativeCamera.setRenderMask(getNative(), renderMask);
    }

    /**
     * Add a {@linkplain GVRPostEffect post-effect} to this camera's render
     * chain.
     * 
     * Post-effects are GL shaders, applied to the texture (hardware bitmap)
     * containing the rendered scene graph. Each post-effect combines a shader
     * selector with a set of parameters: This lets you pass different
     * parameters to the shaders for each eye.
     * 
     * @param postEffectData
     *            Post-effect to append to this camera's render chain
     */
    public void addPostEffect(GVRPostEffect postEffectData) {
        NativeCamera.addPostEffect(getNative(), postEffectData.getNative());
    }

    /**
     * Remove (all instances of) a {@linkplain GVRPostEffect post-effect} from
     * this camera's render chain
     * 
     * @param postEffectData
     *            Post-effect to remove.
     */
    public void removePostEffect(GVRPostEffect postEffectData) {
        NativeCamera.removePostEffect(getNative(), postEffectData.getNative());
    }
}

class NativeCamera {
    static native float getBackgroundColorR(long camera);

    static native void setBackgroundColorR(long camera, float r);

    static native float getBackgroundColorG(long camera);

    static native void setBackgroundColorG(long camera, float g);

    static native float getBackgroundColorB(long camera);

    static native void setBackgroundColorB(long camera, float b);

    static native float getBackgroundColorA(long camera);

    static native void setBackgroundColorA(long camera, float a);

    static native int getRenderMask(long camera);

    static native void setRenderMask(long camera, int renderMask);

    static native void addPostEffect(long camera, long postEffectData);

    static native void removePostEffect(long camera, long postEffectData);
}
