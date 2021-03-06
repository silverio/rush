//****************************************************************************/
//  File:  JRenderServerGL.h
//  Desc:
//****************************************************************************/
#ifndef __JRENDERSERVERGL_H__
#define __JRENDERSERVERGL_H__

#include "irenderserver.h"
#include "mat3.h"
#include "mat4.h"
#include "frame.h"
#include "timer.h"

const int c_QuadIBSize = 65536*2*6;
//****************************************************************************/
//  Class:  JRenderServerGL
//  Desc:
//****************************************************************************/
class JRenderServerGL : public JObject, public IRenderServer
{
public:
                            JRenderServerGL ();
    virtual                 ~JRenderServerGL();
    virtual bool            Shut            ();
    virtual void            Init            ();
    virtual bool            IsInited        () const { return m_bInited; }
    virtual const Frame&    GetViewport     () const { return m_Viewport; }
    virtual void            SetViewport     ( const Frame& rc );
    virtual bool            StartFrame      ();
    virtual void            EndFrame        ();
    virtual void            ClearViewport   ( uint32_t color );
    virtual void            ClearDepthStencil( bool bDepth, bool bStencil );
    virtual void            ShowCursor      ( bool bShow = true );

    virtual int             GetVDeclID      ( const VertexDeclaration& vdecl );

    virtual void            SetAmbient      ( uint32_t color );
    virtual int             AddPointLight   ( const Vec3& pos, float radius, float falloff,
                                              uint32_t diffuse, uint32_t specular, bool bPerPixel );
    virtual int             AddDirLight     ( const Vec3& dir, uint32_t diffuse, uint32_t specular, bool bPerPixel );

    virtual int             GetFPS          () const { return (int)m_FPS; }
    virtual uint32_t           GetCurFrame     () const { return m_CurFrame; }

    virtual void            SetViewTM       ( const Mat4& tm );
    virtual void            SetWorldTM      ( const Mat4& tm );
    virtual void            SetProjTM       ( const Mat4& tm );

    virtual const Mat4&     GetViewTM       () const { return m_ViewTM; }
    virtual const Mat4&     GetWorldTM      () const { return m_WorldTM; }
    virtual const Mat4&     GetProjTM       () const { return m_ProjTM; }

    virtual void            SetTextureTM    ( const Mat4& tm );
    virtual const Mat4&     GetTextureTM    () const { return m_TextureTM; }

    virtual void            SetColorTint    ( uint32_t color );

    virtual int             GetTextureID    ( const char* texName );
    virtual bool            GetTextureProp  ( int texID, TextureProperties& texProp );
    virtual void            SetTexture      ( int texID, int stage );
    virtual void            DeleteTexture   ( int texID );
    virtual int             CreateTexture   ( const TextureProperties& texProp );
    virtual uint8_t*        LockTexture     ( int texID, const Frame& rect, int level = 0, uint32_t* pStride = NULL );
    virtual bool            UnlockTexture   ( int texID, int level = 0 );
    virtual bool            CopyTexture     ( int destID, int srcID, const Frame* rct = NULL, int nRect = 0 );


    virtual int             GetShaderID     ( const char* name, bool bReload = false );
    virtual int             GetConstantID   ( int shaderID, const char* name );
    virtual bool            SetConstant     ( int shaderID, int constID, const Mat4& val );
    virtual bool            SetConstant     ( int shaderID, int constID, float x, float y, float z, float w );
    virtual bool            SetConstant     ( int shaderID, int constID, float val );

    virtual void            SetShader       ( int shID );

    virtual void            ReloadResources ();

    virtual int             CreateRenderTarget  ( const char* texName, int w, int h, ColorFormat fmt = ColorFormat_RGB565 );
    virtual int             CreateDepthStencil  ( const char* texName, int w, int h, DepthFormat fmt = DepthFormat_D16 );
    virtual bool            SetRenderTarget     ( int texID, int dsID = -1 );
    virtual int             GetRenderTarget     ( int* pDSID = NULL ) const { if (pDSID) *pDSID = m_DSID; return m_RTID; }
    virtual bool            SaveTexture         ( int texID, const char* fname );

    virtual int             CreateVB        ( const char* name, int numBytes, bool bStatic = false );
    virtual int             GetVBID         ( const char* name );
    virtual uint8_t*        LockVB          ( int vbID, int firstByte, int numBytes );
    virtual bool            UnlockVB        ( int vbID );
    virtual bool            SetVB           ( int vbID, int vdeclID, int stream );
    virtual int             ClearVB         ( int vbID );
    virtual bool            DeleteVB        ( int vbID );

    virtual int             CreateIB        ( const char* name, int numBytes, bool bStatic = false );
    virtual int             GetIBID         ( const char* name );
    virtual uint8_t*        LockIB          ( int ibID, int firstByte, int numBytes );
    virtual bool            UnlockIB        ( int ibID );
    virtual bool            SetIB           ( int ibID, int baseIndex );
    virtual int             ClearIB         ( int ibID );
    virtual bool            DeleteIB        ( int ibID );

    virtual bool            Draw            ( int firstIdx, int numIdx, int firstVert, int numVert,
                                                PrimitiveType primType = PrimitiveType_TriangleList );

    virtual bool            CacheIB         ( int ibID, uint8_t* pData, int size, int& iteration, int& firstByte );
    virtual bool            CacheVB         ( int vbID, uint8_t* pData, int size, int stride, int& iteration, int& firstByte );
    virtual int             GetVBIteration  ( int vbID );
    virtual int             GetIBIteration  ( int ibID );

    virtual void            ResetTime       ();

    virtual void            Register        ( IRenderServerClient* pClient );
    virtual void            Unregister      ( IRenderServerClient* pClient );

    bool                    IsFullScreen    () const;
    void                    SetFullScreen   ( bool bFullScreen );

    bool                    IsWireFrame     () const { return m_bWireFrame; }
    void                    SetWireFrame    ( bool bWireFrame );

    int                     GetDIPsPerFrame     () const { return m_CurDIPsPerFrame;       }
    int                     GetPolysPerFrame    () const { return m_CurPolysPerFrame;      }
    int                     GetShadersPerFrame  () const { return m_CurShadersPerFrame;    }
    int                     GetTexturesPerFrame () const { return m_CurTexturesPerFrame;   }
    int                     GetRTsPerFrame      () const { return m_CurRTsPerFrame;        }

    virtual void*           GetRenderSurface    ();
    virtual bool            ResizeBackBuffer    ( int w, int h );

    expose(JRenderServerGL)
    {
        parent(JObject);
        rprop( "FPS",           GetFPS              );
        rprop( "DIPsPF",        GetDIPsPerFrame     );
        rprop( "PolysPF",       GetPolysPerFrame    );
        rprop( "ShadersPF",     GetShadersPerFrame  );
        rprop( "TexturesPF",    GetTexturesPerFrame );
        rprop( "RTsPF",         GetRTsPerFrame      );

        prop( "fullscreen", IsFullScreen, SetFullScreen );
        prop( "WireFrame",  IsWireFrame, SetWireFrame  );

        method( "ReloadResources", ReloadResources );
    }

private:
    Frame                           m_Viewport;

    uint32_t                        m_CurFrame;
    bool                            m_bInited;
    float                           m_FPS;
    bool                            m_bCursorVisible;

    Mat4                            m_ViewTM;
    Mat4                            m_ProjTM;
    Mat4                            m_WorldTM;
    Mat4                            m_TextureTM;
    Timer                           m_Time;
    float                           m_CurTime;

    bool                            m_bWireFrame;

    int                             m_CurShader;

    std::vector<IRenderServerClient*> m_Clients;

    uint32_t                        m_CurFVF;
    int                             m_QuadIB;

    int                             m_BaseIndex;

    uint32_t                        m_Ambience;
    bool                            m_bHasStencil;

    bool                            m_bDeviceLost;

    int                             m_RTID;
    int                             m_DSID;

    int                             m_PoolShaderID;

    int                             m_hWorldTM;
    int                             m_hViewTM;
    int                             m_hProjTM;
    int                             m_hCamPos;
    int                             m_hWorldViewProjTM;
    int                             m_hMtlDiffuse;
    int                             m_hMtlAmbient;
    int                             m_hAmbience;
    int                             m_hSunDirection;
    int                             m_hSunDiffuse;
    int                             m_hSunSpecular;
    int                             m_hTime;

    int                             m_DIPsPerFrame;
    int                             m_PolysPerFrame;
    int                             m_ShadersPerFrame;
    int                             m_TexturesPerFrame;
    int                             m_RTsPerFrame;

    int                             m_CurDIPsPerFrame;
    int                             m_CurPolysPerFrame;
    int                             m_CurShadersPerFrame;
    int                             m_CurTexturesPerFrame;
    int                             m_CurRTsPerFrame;

    void                            InitGlobalConstants();
    void                            SetGlobalConstants();

    void                            ReleaseDefaultRes();
    void                            RecreateDefaultRes();
    void                            CreateQuadIB();

    void                            ResetStats();
}; // class JRenderServerGL

#endif // __JRENDERSERVERGL_H__
