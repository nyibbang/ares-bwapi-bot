/*
 * AresBWAPIBot : A bot for SC:BW using BWAPI library;
 * Copyright (C) 2015 Vincent PALANCHER; Florian LEMEASLE
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include "config.h"
#include "Utils.h"
#ifdef ARES_MSWINDOWS
#include <windows.h>
#include <shlobj.h>
#endif

namespace utils
{

std::string homePath()
{
#ifdef ARES_MSWINDOWS
    WCHAR wpath[MAX_PATH];
    if (!SUCCEEDED(SHGetFolderPathW(0, CSIDL_PROFILE, 0, 0, wpath))) {
        return std::string();
    }
    std::wstring path(wpath);
    return std::string(path.begin(), path.end());
#else
    return std::getenv("HOME");
#endif
}

}
