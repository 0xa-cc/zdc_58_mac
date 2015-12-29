#ifndef _TOKENIZER_ZHCN_H_
#define _TOKENIZER_ZHCN_H_

/*
#ifndef BYTE
#define BYTE char
#define SPH_MAX_WORD_LEN		64
#endif
*/

class CSphTokenizer_zh_CN_UTF8_Private;

class CSphTokenizer_zh_CN_UTF8: public CSphTokenizerTraits<true>
{
public:
								CSphTokenizer_zh_CN_UTF8 ();
								~CSphTokenizer_zh_CN_UTF8();

	virtual void				SetBuffer ( BYTE * sBuffer, int iLength );
	virtual BYTE *				GetToken (int n = 0);
	virtual ISphTokenizer *		Clone ( bool bEscaped ) const;
	virtual bool				IsUtf8 () const { return true; }
	virtual int					GetCodepointLength ( int iCode ) const;
	void setDictPath(const char* path) {	m_dictpath = path; }
	void setDebugMode(int val) { m_bDebug = val; }
	virtual bool					IsSentenceEnd();
	virtual bool					IsKeyWord(BYTE * sBuffer, int iLength );
	virtual int						GetWordWeight (BYTE * sBuffer, int iLength) const;
//protected:
	//BYTE *				m_pBuffer;		///< my buffer
	//BYTE *				m_pBufferMax;	///< max buffer ptr, exclusive (ie. this ptr is invalid, but every ptr below is ok)
	//BYTE *				m_pCur;			///< current position
	//int					m_iOvershortCount;	///< skipped overshort tokens count
	//BYTE				m_sAccum [ 3*SPH_MAX_WORD_LEN+3 ];
protected:
	CSphTokenizer_zh_CN_UTF8_Private* d_;
	//const char* m_dictpath;
	CSphString m_dictpath;
	int m_bDebug;
	void						FlushAccum ();
};

#if USE_LIBICONV

#define GBK_CONVERT_BUFFER_SIZE 512*1024*3
class CSphTokenizer_zh_CN_GBK : public CSphTokenizer_zh_CN_UTF8
{
public:
								CSphTokenizer_zh_CN_GBK ();
								~CSphTokenizer_zh_CN_GBK();
	virtual void				SetBuffer ( BYTE * sBuffer, int iLength );
	virtual int					GetLocalBuffer( BYTE * sBuffer, int iLength, BYTE * sOBuffer);
	//virtual BYTE *			GetToken ();
	virtual ISphTokenizer *		Clone ( bool bEscaped ) const;
protected:
	BYTE m_default_convert_buffer[GBK_CONVERT_BUFFER_SIZE];
	BYTE* m_convert_buffer;
	int m_buffer_size;
};

#endif

#endif

