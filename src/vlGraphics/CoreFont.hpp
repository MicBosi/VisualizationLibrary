/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi                                             */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  Redistribution and use in source and binary forms, with or without modification,  */
/*  are permitted provided that the following conditions are met:                     */
/*                                                                                    */
/*  - Redistributions of source code must retain the above copyright notice, this     */
/*  list of conditions and the following disclaimer.                                  */
/*                                                                                    */
/*  - Redistributions in binary form must reproduce the above copyright notice, this  */
/*  list of conditions and the following disclaimer in the documentation and/or       */
/*  other materials provided with the distribution.                                   */
/*                                                                                    */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   */
/*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            */
/*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  */
/*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    */
/*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      */
/*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    */
/*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     */
/*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      */
/*                                                                                    */
/**************************************************************************************/

#ifndef CoreFont_INCLUDE_ONCE
#define CoreFont_INCLUDE_ONCE

#include <vlCore/Object.hpp>
#include <vlCore/Vector4.hpp>
#include <vlCore/String.hpp>
#include <vlCore/Image.hpp>
#include <vlGraphics/link_config.hpp>
#include <vlGraphics/Texture.hpp>
#include <map>

//-----------------------------------------------------------------------------
struct FT_FaceRec_;
typedef struct FT_FaceRec_*  FT_Face;
//-----------------------------------------------------------------------------
namespace vl
{
  class CoreFont;
  class FontManager;
  //-----------------------------------------------------------------------------
  // Glyph
  //-----------------------------------------------------------------------------
  /**
   * The Glyph associated to a character of a given Font.
  */
  class CoreGlyph: public Object
  {
    VL_INSTRUMENT_CLASS(vl::Glyph, Object)

  private:
    CoreGlyph(const CoreGlyph& other): Object(other)  
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }
    void operator=(const CoreGlyph&){}

  public:
    CoreGlyph(): mFont(NULL), mS0(0), mT0(0), mS1(0), mT1(0), mGlyphIndex(0)/*, mTextureHandle(0)*/, mWidth(0), mHeight(0), mLeft(0), mTop(0) {}

    ~CoreGlyph();

    /*unsigned int textureHandle() const { return mTextureHandle; }
    void setTextureHandle(unsigned int handle) { mTextureHandle = handle; }*/

    int width() const { return mWidth; }
    void setWidth(int width) { mWidth = width; }

    int height() const { return mHeight; }
    void setHeight(int height) { mHeight = height; }

    int left() const { return mLeft; }
    void setLeft(int left) { mLeft = left; }

    int top() const { return mTop; }
    void setTop(int top) { mTop = top; }

    float s0() const { return mS0; }
    void setS0(float s0) { mS0 = s0; }

    float t0() const { return mT0; }
    void setT0(float t0) { mT0 = t0; }

    float s1() const { return mS1; }
    void setS1(float s1) { mS1 = s1; }

    float t1() const { return mT1; }
    void setT1(float t1) { mT1 = t1; }

    const fvec2& advance() const { return mAdvance; }
    void setAdvance(const fvec2& advance) { mAdvance = advance; }

    unsigned int glyphIndex() const { return mGlyphIndex; }
    void setGlyphIndex(unsigned int glyph_index) { mGlyphIndex = glyph_index; }

    const CoreFont* font() const { return mFont; }
    void setFont(CoreFont* font) { mFont = font; }

  protected:
    CoreFont* mFont;
    fvec2 mAdvance;
    float mS0;
    float mT0;
    float mS1;
    float mT1;
    unsigned int mGlyphIndex;
    int mWidth;
    int mHeight;
    int mLeft;
    int mTop;
  };
  //-----------------------------------------------------------------------------
  // Font
  //-----------------------------------------------------------------------------
  /**
   * A font to be used with a Text renderable.
  */
  class VLGRAPHICS_EXPORT CoreFont: public Object
  {
    VL_INSTRUMENT_CLASS(vl::Font, Object)

    friend class CoreText;
    friend class Text;
    friend class FontManager;
    
    //! Assignment operator
    void operator=(const CoreFont&) { VL_TRAP() } // should never get used
    
    //! Copy constructor.
    CoreFont(const CoreFont& other): Object(other)/*, mTextureHandle(0) */{ VL_TRAP() } // should never get used
    
    //! Constructor.
    CoreFont(FontManager* fm = NULL);
    
    //! Constructor. The specified 'font_file' is immediately loaded.
    CoreFont(FontManager* fm, const String& font_file, int size );

  public:
    //! Destructor.
    ~CoreFont();

    //! Less-than operator.
    bool operator<(const CoreFont& other) const
    {
      if (filePath() != other.filePath())
        return filePath() < other.filePath();
      else
        return size() < other.size();
    }

    //! The font's file path.
    const String& filePath() const { return mFilePath; }
    
    //! Loads a font using fileSystem()::locateFile().
    void loadFont(const String& path);

    //! The size of the font.
    int size() const { return mSize; }
    
    //! The size of the font.
    void setSize(int size);

    //! Returns (and eventually creates) the Glyph* associated to the given character.
    CoreGlyph* glyph(int character);
    
    //! Whether the font rendering should use linear filtering or not.
    void setSmooth(bool smooth);
    
    //! Whether the font rendering should use linear filtering or not.
    bool smooth() const { return mSmooth; }

    //! Releases the FreeType's FT_Face used by a Font.
    void releaseFreeTypeData();

    //! The FontManager associated to this Font used to acquire/release FreeType resources.
    void setFontManager(FontManager* fm) { mFontManager = fm; }
    
    //! The FontManager associated to this Font used to acquire/release FreeType resources.
    const FontManager* fontManager() const { return mFontManager; }

    //! The FontManager associated to this Font used to acquire/release FreeType resources.
    FontManager* fontManager() { return mFontManager; }

    //! Whether FT_Load_Char() should be called with FT_LOAD_FORCE_AUTOHINT (default) or FT_LOAD_DEFAULT.
    //! There isn't a "best" option for all the fonts, the results can be better or worse depending on the particular font loaded.
    bool freeTypeLoadForceAutoHint() const { return mFreeTypeLoadForceAutoHint; }

    //! Whether FT_Load_Char() should be called with FT_LOAD_FORCE_AUTOHINT (default) or FT_LOAD_DEFAULT.
    //! There isn't a "best" option for all the fonts, the results can be better or worse depending on the particular font loaded.
    void setFreeTypLoadForceAutoHint(bool enable) { mFreeTypeLoadForceAutoHint = enable; }
	
	//! Create the texture of the atlas
	void createTexture();
	
	//! Get the atlas texture
	Texture* getAtlas() { return mAtlas.get(); }

  protected:
    FontManager* mFontManager;
    String mFilePath;
    std::map< int, ref<CoreGlyph> > mGlyphMap;
    FT_Face mFT_Face;
    std::vector<char> mMemoryFile;
    int mSize;
    float mHeight;
    bool mSmooth;
    bool mFreeTypeLoadForceAutoHint;
	ref< Texture > mAtlas;
  
  };
  //-----------------------------------------------------------------------------
}

#endif
