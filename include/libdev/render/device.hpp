#pragma once

#include "base/base.hpp"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include "render/internal/trigroup.hpp"
#include "render/texture.hpp"

class SysPathName;

class MexQuad3d;
class MexPoint2d;
class MexPoint3d;

class RenDisplay;
class RenColour;
class RenCamera;
class RenLight;
class RenMaterial;
class RenCapabilities;
class RenStats;
class RenSurface;
class RenISurfBody;

class RenIDeviceImpl;
class RenIVertexData;

// A device object represents a rendering target.  It orchestrates
// the rendering process.

class RenDevice final
{
public:
    // Selects a D3D driver; uses the display to create front and back
    // buffers; loads a palette, if necessary; possibly changes the display
    // resolution; and generally sets everything up for 3D rendering.
    // Calls useDevice(this).
    // PRE(Ren::initialised());
    // PRE(MexCoordSystem::instance().isSet());
    RenDevice(RenDisplay*);
    ~RenDevice();

    // We can fail to start the rendering in which case false is returned.  If
    // clearBack is false, then the back buffer isn't cleared (but z is).
    // Calls useDevice(this).
    // PRE(!rendering());
    // POST(implies(result, idleRendering()));
    bool startFrame();

    // Sets the rendering parameters appropriately for 2D gui stuff.
    // PRE(idleRendering());
    // POST(rendering2D());
    void start2D();
    // PRE(rendering2D());
    void end2D();

    // Sets the rendering parameters appropriately for the 3D world.
    // PRE(idleRendering());
    // POST(rendering3D());
    void start3D(bool clearBack = true);
    // PRE(rendering3D());
    void end3D();

    // PRE(rendering());
    // PRE(!doingBackground_);
    void startBackground(double yon);

    // PRE(idleRendering());
    // POST(!rendering());
    // POST(!rendering3D() and !rendering2D());
    // POST(!doingBackground_);
    void endFrame();

    // True within calls of startFrame() and endFrame().
    bool rendering()   const;
    // True within calls of start2D() and end2D().
    bool rendering2D() const;
    // True within calls of start3D() and end3D().
    bool rendering3D() const;
    // True when rendering() is true and when rendering2D() and rendering3D() are false.
    bool idleRendering() const;

    // Immediately clear all display surfaces to a given flat colour.  If no
    // colour is specified, the current background colour is used.  This is
    // useful for getting everything into a known state before starting to
    // compose a scene.  The entire screen area is cleared.  If a cursor is
    // (or was) displayed any saved areas are discarded.
    void clearAllSurfaces(const RenColour&);			// PRE(!rendering());
    void clearAllSurfaces();							// PRE(!rendering());

    void reset();
    virtual void setMaterialHandles(const RenMaterial& mat);

    // If the display mode changes, the device must be re-initialised.  This is
    // initially called by the ctor.  The client is responsible for calling
    // setViewport so that the viewport fits on the new display.
    bool reinitializeDisplayAndCreateGlContext();
    bool fitToDisplay(RenDisplay* pDisplay) __attribute__ ((deprecated));

    // This sets the sub-area of the screen which is used for 3D rendering.  The
    // remainder of the back-buffer is accessible thru the RenSurface interface.
    // The device only clears the area inside this rectangle, any remaining parts
    // of the buffer (borders, gui, etc.) are the client's responsibility.
    // Defaults to full-screen if you never call this method.
    // PRE(!rendering3D());
    void setViewport(int left, int top, int width, int height);

    // Use another camera.  At present, multiple viewports on one device
    // aren't supported, so you can't switch cameras mid-frame.
    // PRE(cam);
    // PRE(!rendering3D());
    // PRE(cam->hitherClipDistance() < cam->yonClipDistance());
    void useCamera(RenCamera* cam);

    // Which camera is currently in use?
    RenCamera* currentCamera() const;

    //Return the coordinates of screenPosition (defined in screen 2d pixel space)
    //in the 3d world coordinate frame of the camera
    MexPoint3d screenToCamera( const MexPoint2d& screenPosition ) const;

    //Return the coordinates of worldPosition (defined in in the 3d world coordinate frame of the camera)
    //in screen 2d pixel space
    MexPoint2d cameraToScreen( const MexPoint3d& worldPosition ) const;

    // Modify the list of objects which can potentially light the scene.
    // PRE(light);
    void    addLight(RenLight* light);
    void removeLight(RenLight* light);

    // The default value is a white light of 0.3 intensity.
    const RenColour& ambient() const;
    void ambient(const RenColour&);

    // This has the effect of turning all lights off.  Polygons will be drawn with
    // ambient and self-luminous lighting only.  If a client knows that a mesh is
    // all black or self-luminous, it will be drawn much more efficiently if
    // lighting is diabled.
    void disableLighting();
    void enableLighting();
    bool lightingEnabled() const;

    // Although there can be more than one Direct3D device in existance,
    // we should never need two simultaneously.  The client is expected to
    // set a current device before using any other rendering functionality.
    // N.B. a device *must* be set before loading meshes or textures.
    static void useDevice(RenDevice* d);
    static RenDevice* current();

    void backgroundColour(const RenColour&);
    const RenColour& backgroundColour() const;

    // Densities of around 0.05 work.
    // PRE(start > 0); PRE(start < end); PRE(density >= 0 && density <= 1);
    void fogOn(float start, float end, float density);
    void fogOn();
    void fogOff();
    const RenColour& fogColour() const;
    void fogColour(const RenColour& newFogColour);
    void fogEnd(float end);

    // These effect temporary changes to the fog parameters.  The status quo
    // can be restored by calling restoreFog.  Primarily for W4dBackground.
    // If restoreFog isn't called, endFrame restores the parameters anyway.
    void disableFog();
    void overrideFog(float start, float end, float density);
    void overrideFog(float start, float end, float density, const RenColour&);
    void restoreFog();

    // Fog query methods.
    bool isFogOn() const;
    float fogStart() const;
    float fogEnd() const;
    float fogDensity() const;

    // The dimensions of the display, window or whatever.
    int windowWidth () const;
    int windowHeight() const;

    // Point visibility test
    // PRE(currentCamera());
    bool canSee(const MexPoint3d& pt) const;
    // Quad visibility test which supports domain/portal culling.
    // PRE(currentCamera());
    bool canSee(const MexQuad3d& quad) const;

    // Quad visibility test which supports domain/portal culling.
    // Uses the given camera instead of currentCamera().
    // PRE(currentCamera());
    bool canSee(const RenCamera* cam, const MexQuad3d& quad) const;

    // Display an image on this device's surface -- typically this would be
    // used for displaying a "please wait" screen.
    void displayImage(const SysPathName& pathName);

    // Interference covers a lot of the screen and looks like a badly tunned TV.
    // High interference values will entirely obliterate the image.  On the other
    // hand, static (see below) hardly obscures anything.  Interference carries a
    // fairly high performance penalty in terms of screen over-writes.
    // Calling interferenceOff is equivalent to interferenceOn(0) just more wordy.
    void interferenceOn(double amount);		// PRE(amount >=0 && amount <= 1);
    void interferenceOff();
    double interferenceAmount() const;
    bool isInterferenceOn() const;

    // Adds a random speckles to the frame buffer.  TBD: perhaps this should
    // belong in RenCamera, like colourFilter?
    void staticOn();
    void staticOff();
    bool isStaticOn() const;

    // The display on which this device is rendered.
    RenDisplay* display();
    const RenDisplay* display() const;

    bool setHighestAllowedDisplayMode();

    // Create surfaces which provide access to the front and back buffers.
    RenSurface backSurface();
    RenSurface frontSurface();
    const RenSurface backSurface()  const;
    const RenSurface frontSurface() const;

    // Enable edge anti-aliasing.  In the only currently available implementation
    // (nVidia TNT), this is *very* slow.  It should probably only be turned on
    // for screenshots.  There will be no effect if the h/w doesn't support it.
    void antiAliasingOn(bool);
    bool antiAliasingOn() const;

    // Clients can print any debugging text here.  Any text will be overlayed
    // on the 3D graphics.  The 3D is drawn first, then any internal text such
    // as frame rate, then the client's text below that.
    std::ostream& out();

    // Display the given text as a horizonal scrolling banner.
    void horizontalBannerText(const char* text, int nChars, double charsPerSecond);

    // The debug text appears at these co-ordinates.  (Well actually, the internal
    // stuff like stats appears first, then the debug text below that).
    void debugTextCoords(int x, int y);
    void debugTextCoords(int* x, int* y) const;

    friend std::ostream& operator <<( std::ostream& o, const RenDevice& t );

    bool activate();

    // Statistics describing the performance of the render system.  The return
    // value can be null, indicating that statistics are not being gathered.
    RenStats* statistics();
    const RenStats* statistics() const;

    // Features supported by this rendering system.
    const RenCapabilities& capabilities() const;

    //colour filter

    class Filter
    {
    public:
        Filter(RenDevice* pDevice, const RenColour& col);	// call setFilter
        ~Filter();							// call resetFilter

    private:
        RenDevice * const pDevice_;

        Filter();
        Filter(const Filter&);
        const Filter& operator=(const Filter&) ;
    } ;

    RenIDeviceImpl& impl();
    const RenIDeviceImpl& impl() const;

    const GLuint loadShaders(const char* vertexPath, const char* fragmentPath);

    void renderScreenspace(const RenIVertex* vertices, const size_t nVertices, GLenum mode, const int targetW = 0, const int targetH = 0)
    {
        renderScreenspace(vertices, nVertices, mode, targetW, targetH, glTextureEmptyID_);
    };

    void renderScreenspace(const RenIVertex* vertices, const size_t nVertices, const RenMaterial& mat, GLenum mode, const int targetW, const int targetH )
    {
        renderScreenspace(vertices, nVertices, mode, targetW, targetH, mat.texture().handle());
    };
    void renderSurface(const RenISurfBody* surf, const Ren::Rect& srcArea, const Ren::Rect& dstArea, const uint32_t targetW = 0, const uint32_t targetH = 0, const uint32_t colour = 0xFFFFFFFF);
    void renderPrimitive(const RenIVertex* vertices, const size_t nVertices, const RenMaterial& mat, const GLenum mode = GL_TRIANGLE_FAN);
    void renderIndexed(const RenIVertex* vertices,  const size_t nVertices, const Ren::VertexIdx* indices, const size_t nIndices, const RenMaterial& mat, GLenum mode);
    void renderIndexedScreenspace(const RenIVertex* vertices,  const size_t nVertices, const Ren::VertexIdx* indices, const size_t nIndices, const RenMaterial& mat, GLenum mode);

    void setModelMatrix(glm::mat4& model)
    {
        model_ = model;
    }

    const glm::mat4& getModelMatrix()
    {
        return model_;
    }

    const glm::mat4& getProjectionMatrix()
    {
        return projection_;
    }

    const glm::mat4& getViewMatrix()
    {
        return view_;
    }
    void renderToTextureMode(const GLuint targetTexture, uint32_t viewPortW, uint32_t viewPortH);

    //private:
    void renderScreenspace(const RenIVertex* vertices, const size_t nVertices, GLenum mode, const int targetW, const int targetH, const GLuint texture);
private:
    void createViewport();
    void updateMatrices();
    void displayStats();
    void updateViewMatrix(glm::mat4& view);
    void updateProjMatrix(double hither, double yon, double h);
    void setFog(float, float, float, const RenColour&);
    void addStatic();
    void addInterference();
    void commonEndFrame();

    friend class Filter;
    void setFilter(const RenColour&);
    void resetFilter();

    // Sets the rendering parameters appropriately for the 3D world.
    // Draws all alpha polygons. start3D must have been called first.
    // PRE(rendering());
    // PRE(rendering3D());
    void flush3DAlpha();

    // Change the clipping planes mid-frame.  The effect lasts until endFrame
    // is called.  Intended for drawing backgrounds.
    // PRE(hither < yon);
    void overrideClipping(double hither, double yon);

    RenIDeviceImpl* pImpl_{nullptr};

    GLuint glProgramID_GIU2D_{0}, glProgramID_Standard_{0}, glProgramID_Billboard_{0};

    GLuint gl2DVertexBufferID_{0}, glVertexUVID_{0}, glVertexPosition_screenspaceID_{0}, glVertexColour_screenspaceID_{0}, glScreenspaceID_{0};

    GLuint gl2DUniformID_{0}, glTextureSamplerID_{0}, glTextureSamplerBillboardID_{0};
    GLuint glModelMatrixID_{0}, glViewMatrixID_{0}, glProjectionMatrixID_{0};
    GLuint glFogColourID_{0}, glFogParamsID_{0};

    GLuint glVertexPosition_modelspaceID_{0}, glVertex_modelspaceUVID_{0}, glVertexColour_modelspaceID_{0}, glVertexDataBufferID_{0}, glElementBufferID_{0};
    GLuint glViewProjMatrix_BillboardID_{0}, glVertexPosition_BillboardID_{0}, glVertex_BillboardUVID_{0}, glVertexColour_BillboardID_{0}, glVertexDataBufferBillboardID_{0}, glElementBufferBillboardID_{0};
    GLuint glTextureEmptyID_{0}, glOffscreenFrameBuffID_{0};

    SDL_GLContext SDLGlContext_{nullptr};

    glm::mat4 model_{};
    glm::mat4 view_{};
    glm::mat4 projection_{};
    glm::vec3 fogColour_{};
    glm::vec3 fogParams_{};

    // Operations deliberately revoked
    RenDevice( const RenDevice& );
    RenDevice& operator =( const RenDevice& ) ;
    bool operator ==( const RenDevice& ) ;
};
