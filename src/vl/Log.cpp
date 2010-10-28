/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
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

#include <vl/Log.hpp>
#include <vl/checks.hpp>
#include <vl/VisualizationLibrary.hpp>
#include <cstdio>

using namespace vl;

//-----------------------------------------------------------------------------
ref<Log> Log::mLogger;
//-----------------------------------------------------------------------------
// Log
//-----------------------------------------------------------------------------
void Log::print(ELogLevel level, const String& log)
{
  EVerbosityLevel verbosity = VisualizationLibrary::settings()->verbosityLevel();
  if (mLogger && verbosity != vl::VEL_VERBOSITY_SILENT)
  {
    switch(level)
    {
    case LogBug:
    case LogError:
    case LogWarning:
      if (verbosity >= vl::VEL_VERBOSITY_ERROR)
        mLogger->printImplementation(level, log);
      break;
    case LogNormal:
    case LogInfo:
      if (verbosity >= vl::VEL_VERBOSITY_NORMAL)
        mLogger->printImplementation(level, log);
      break;
    case LogDebug:
      if (verbosity >= vl::VEL_VERBOSITY_DEBUG)
        mLogger->printImplementation(level, log);
      break;
    }
  }
}
//-----------------------------------------------------------------------------
void Log::force_print(const String& log) { if (mLogger) mLogger->printImplementation(LogNormal, log); }
//-----------------------------------------------------------------------------
void Log::print(const String& log) { print(LogNormal, log); }
//-----------------------------------------------------------------------------
void Log::debug(const String& log) { print(LogDebug, log); }
//-----------------------------------------------------------------------------
void Log::info(const String& log) { print(LogInfo, log); }
//-----------------------------------------------------------------------------
void Log::warning(const String& log) { print(LogWarning, log); }
//-----------------------------------------------------------------------------
void Log::error(const String& log) { print(LogError, log); }
//-----------------------------------------------------------------------------
void Log::bug(const String& log) { print(LogBug, log); }
//-----------------------------------------------------------------------------
// StandardLog
//-----------------------------------------------------------------------------
void StandardLog::printImplementation(ELogLevel level, const String& in_log)
{
  if (in_log.empty())
    return;

  String log = in_log;

  switch(level)
  {
    case LogNormal: break;
    case LogDebug:   log = "debug: "   + log; break;
    case LogInfo:    log = "info: "    + log; break;
    case LogWarning: log = "warning: " + log; break;
    case LogError:   log = "error: "   + log; break;
    case LogBug:     log = "bug: "     + log; break;
  }

  fprintf( stdout,"%s", log.toStdString().c_str() );

  if (!logFile().empty())
  {
    FILE*fout = fopen( logFile().toStdString().c_str(), "at" );
    if (fout)
    {
      fprintf( fout, "%s", log.toStdString().c_str() ); 
      fclose( fout );
    }
  }
}
//-----------------------------------------------------------------------------
