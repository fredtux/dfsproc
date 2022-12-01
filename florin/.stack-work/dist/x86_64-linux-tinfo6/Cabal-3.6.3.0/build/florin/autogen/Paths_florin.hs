{-# LANGUAGE CPP #-}
{-# LANGUAGE NoRebindableSyntax #-}
{-# OPTIONS_GHC -fno-warn-missing-import-lists #-}
{-# OPTIONS_GHC -w #-}
module Paths_florin (
    version,
    getBinDir, getLibDir, getDynLibDir, getDataDir, getLibexecDir,
    getDataFileName, getSysconfDir
  ) where


import qualified Control.Exception as Exception
import qualified Data.List as List
import Data.Version (Version(..))
import System.Environment (getEnv)
import Prelude


#if defined(VERSION_base)

#if MIN_VERSION_base(4,0,0)
catchIO :: IO a -> (Exception.IOException -> IO a) -> IO a
#else
catchIO :: IO a -> (Exception.Exception -> IO a) -> IO a
#endif

#else
catchIO :: IO a -> (Exception.IOException -> IO a) -> IO a
#endif
catchIO = Exception.catch

version :: Version
version = Version [0,1,0,0] []

getDataFileName :: FilePath -> IO FilePath
getDataFileName name = do
  dir <- getDataDir
  return (dir `joinFileName` name)

getBinDir, getLibDir, getDynLibDir, getDataDir, getLibexecDir, getSysconfDir :: IO FilePath



bindir, libdir, dynlibdir, datadir, libexecdir, sysconfdir :: FilePath
bindir     = "/tux/dfsproc/florin/.stack-work/install/x86_64-linux-tinfo6/2cb220e6aa3da5432f95749968c79162c5fe4b454d5b325e1de04546a9f67a16/9.2.5/bin"
libdir     = "/tux/dfsproc/florin/.stack-work/install/x86_64-linux-tinfo6/2cb220e6aa3da5432f95749968c79162c5fe4b454d5b325e1de04546a9f67a16/9.2.5/lib/x86_64-linux-ghc-9.2.5/florin-0.1.0.0-5kIGUMTniNQI5qjfCzcCKi-florin"
dynlibdir  = "/tux/dfsproc/florin/.stack-work/install/x86_64-linux-tinfo6/2cb220e6aa3da5432f95749968c79162c5fe4b454d5b325e1de04546a9f67a16/9.2.5/lib/x86_64-linux-ghc-9.2.5"
datadir    = "/tux/dfsproc/florin/.stack-work/install/x86_64-linux-tinfo6/2cb220e6aa3da5432f95749968c79162c5fe4b454d5b325e1de04546a9f67a16/9.2.5/share/x86_64-linux-ghc-9.2.5/florin-0.1.0.0"
libexecdir = "/tux/dfsproc/florin/.stack-work/install/x86_64-linux-tinfo6/2cb220e6aa3da5432f95749968c79162c5fe4b454d5b325e1de04546a9f67a16/9.2.5/libexec/x86_64-linux-ghc-9.2.5/florin-0.1.0.0"
sysconfdir = "/tux/dfsproc/florin/.stack-work/install/x86_64-linux-tinfo6/2cb220e6aa3da5432f95749968c79162c5fe4b454d5b325e1de04546a9f67a16/9.2.5/etc"

getBinDir     = catchIO (getEnv "florin_bindir")     (\_ -> return bindir)
getLibDir     = catchIO (getEnv "florin_libdir")     (\_ -> return libdir)
getDynLibDir  = catchIO (getEnv "florin_dynlibdir")  (\_ -> return dynlibdir)
getDataDir    = catchIO (getEnv "florin_datadir")    (\_ -> return datadir)
getLibexecDir = catchIO (getEnv "florin_libexecdir") (\_ -> return libexecdir)
getSysconfDir = catchIO (getEnv "florin_sysconfdir") (\_ -> return sysconfdir)




joinFileName :: String -> String -> FilePath
joinFileName ""  fname = fname
joinFileName "." fname = fname
joinFileName dir ""    = dir
joinFileName dir fname
  | isPathSeparator (List.last dir) = dir ++ fname
  | otherwise                       = dir ++ pathSeparator : fname

pathSeparator :: Char
pathSeparator = '/'

isPathSeparator :: Char -> Bool
isPathSeparator c = c == '/'
