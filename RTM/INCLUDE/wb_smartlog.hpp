/** \file wb_smartlog.h
 *  \brief Zaawansowane logowanie aplikacji, wraz ze stosem (opcjonalnie)
 *         ---------------------------------------------------------------
 *  \author Wojciech Borkowski @ Institut for Social Studies, University of Warsaw
 *  \date   2023-01-23 (last modification)
 *  \details ... ...
 *           ... ...
 *  \note Like many others wbrtm files created around year 1996.
 *  \ingroup OBSOLETE
 */
#if !defined(AFX_WB_SMARTLOG_H__C0EDBBE3_72CF_11D5_AB82_444553540000__INCLUDED_)
#define AFX_WB_SMARTLOG_H__C0EDBBE3_72CF_11D5_AB82_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <iostream>

#include "wb_ptr.hpp" //class wb_pchar
#include "var_contr.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief Klasa zaawansowanego logowania aplikacji
class wb_smartlog  
{
public:
    /// \brief Konstruktor pozwalający na podanie nazwy pliku.
	wb_smartlog(const char* logname=NULL);

    /// \brief Zmiana nazwy pliku logu.
    /// O ile nie wykonano już pierwszego zapisu do pliku logu
	bool SetName(const char* logname);

    /// \brief Wirtualny destruktor konieczny bo są metody wirtualne.
	virtual ~wb_smartlog();
	
	virtual double GetTime();	 ///< Daje czas w postaci zegara procesora (sek. od rozpoczęcia procesu)

	virtual	int	CurrCallLevel(); ///< Daje aktualny poziom wywołania - ze śledzenia przez `block` markery

    virtual const char* MyName();

	ostream& MyStream();

    /// \brief Sprawdzenie aktualnego poziomu logowania
	bool ifTlog(int level) { return level<=log_level; }

	/// \brief Informuje czy wywołać zapis. DO UŻYCIA POPRZEZ MAKRO!!!
	bool IfMessage(int level,const char* file,int line=0)
	{
		if(level<=log_level)
		{
			{
			const char* rfile=strrchr(file,SLASH_FOR_LOG);
			if(rfile!=NULL)
				file=rfile;
			}

			ostream& pom=MyStream();
			pom<<endl<<GetTime();
			if(use_fname && filename.OK())
				pom<<'\t'<<file<<":"<<line<<"; "; //Zapis nazwy pliku wywołującego LOG
			pom<<'\t'<<level<<". ";
			for(int i=0;i<level;i++)pom<<SEPAR; //Wcięcie
			return true;
		}
		else
		{
			return false;
		}
	}

	/// \brief Interface do zmiany poziomu "logowania"
	/// \note Do używania poprzez MAKRO: `LOCAL_CHANGE_LOG_LEVEL`
	class Lock:public przechwytywacz<int,int>
	{
	public:
		Lock(int val):
			przechwytywacz<int,int>(wb_smartlog::log_level,val){}
		~Lock(){}
	};

    /// ???
	class Block_checker
	{
		wb_smartlog& MyLog;
        int MyLevel;
        const char* BlockName;
        const char* Name;
        int Line;
	public:
		Block_checker(wb_smartlog& mylog,int mylevel,const char* blockname,const char* name,int line):
		  MyLog(mylog),BlockName(blockname),Name(name),Line(line),MyLevel(mylevel)
		  {
			  if(mylog.IfMessage(mylevel,name,line) )
			  {
				  ostream& o=mylog.MyStream();
				  o<<"ENTER BLOCK("<<wb_smartlog::call_level<<")";
				  for(int i=0;i<wb_smartlog::call_level;i++)o<<SEPAR; //Wcięcie
				  o<<": "<< blockname <<flush; 
			  }
			  wb_smartlog::call_level++;
		  }
		  
		  ~Block_checker()
		  {
			  wb_smartlog::call_level--;
			  if(MyLog.IfMessage(MyLevel,Name,Line) )
			  {
				  ostream& o=MyLog.MyStream();
				  o<<"LEAVE BLOCK("<<wb_smartlog::call_level<<")";
				  for(int i=0;i<wb_smartlog::call_level;i++)o<<SEPAR; //Wcięcie
				  o<<": "<< BlockName <<flush; 
			  }
		  }
	};

private:
	wb_pchar filename;		///< Do zapamiętania pliku
	ostream* ptrToMyStream;	///< Strumień do wypluwania komunikatów logu

	bool	Connect();		///< Wiąże log ze strumieniem
	bool	Disconnect();	///< Odwiązuje log od strumienia

	//DONT USE DIRECTLY!!!
	static const char*  SEPAR;  ///< Czym robić wcięcia itp
	static const char	SLASH_FOR_LOG; ///< Co separuje nazwy w strukturze katalogów
	static int		use_fname;  ///< Czy zapisywać nazwę z __FILE__ (tylko do logu plikowego)
	static int		call_level; ///< Głębokość wywołań funkcji liczona przez Block_checker'y

public:
    static const char* Sep() { return SEPAR; } ///< Odczyt separatora używanego do wcięć

	static int		log_level; // To musi byc public ze względu na Lock
                               // no i sterowanie z menu aplikacji i z parameytrów !!!

    friend class Lock;
    friend class Block_checker;
};

} //namespace wbrtm

extern  wbrtm::wb_smartlog TheApplicationLog; //Domyślny log aplikacji, domyślnie powiązany z "clog"

//Przestawia lokalnie i samo-odwracalnie wartość poziomu logowania
//#define _LINE_ __LINE__ identifier
//#define LOCAL_CHANGE_LOG_LEVEL( _p_ ) wb_smartlog::Lock    SetLogLevel ## _LINE_ ( (_p_) );

#define LOCAL_CHANGE_LOG_LEVEL( _p_ ) wbrtm::wb_smartlog::Lock    SetLogLevel( (_p_) );

#define MARK_LOCAL_BLOCK( _l_ , _p_ ) wbrtm::wb_smartlog::Block_checker ThisBlock( TheApplicationLog ,(_l_), _p_ , __FILE__ , __LINE__ )

#define ALOG( ALOG_OBJECT_ , ALOG_LEVEL_ , ALOG_MESSAGE_ ) { if((ALOG_OBJECT_).IfMessage((ALOG_LEVEL_),__FILE__,__LINE__) )\
																{(ALOG_OBJECT_).MyStream()<<(ALOG_OBJECT_).Sep()  ALOG_MESSAGE_ <<flush; }}

#define IFALOG( ALOG_OBJECT_ , ALOG_LEVEL_ )		((ALOG_OBJECT_).IfMessage( (ALOG_LEVEL_) ,(__FILE__) , (__LINE__) ))

#define LOGAS( ALOG_OBJECT_ )						(ALOG_OBJECT_).MyStream()


#define TLOG( TLOG_LEVEL_ , TLOG_MESSAGE_ )			ALOG( (TheApplicationLog) , (TLOG_LEVEL_) , TLOG_MESSAGE_ )

#define IFTLOG( TLOG_LEVEL_ )						IFALOG( (TheApplicationLog) , (TLOG_LEVEL_) )

#define IFTL( TLOG_LEVEL_ )                         if(TheApplicationLog.ifTlog( TLOG_LEVEL_ ))

#define LOGTS()										LOGAS( (TheApplicationLog) )

#ifndef NDEBUG	//JAK w assert.h !!!
#define DEBUG_LOG( DLOG_LEVEL_ , DLOG_MESSAGE_ ) TLOG( (DLOG_LEVEL_) , DLOG_MESSAGE_ )
#define STACK_LOG( SLOG_MESSAGE_ )				 TLOG( (TheApplicationLog.CurrCallLevel()), SLOG_MESSAGE_ 	)
#else
#define DEBUG_LOG( DLOG_LEVEL_ , DLOG_MESSAGE_ ) {  }
#define STACK_LOG( SLOG_MESSAGE_ )				 {  }
#endif

/* ******************************************************************/
/*                WBRTM  version 2022 for GuestXR                   */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif// !defined(AFX_WB_SMARTLOG_H__C0EDBBE3_72CF_11D5_AB82_444553540000__INCLUDED_)
