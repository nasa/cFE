/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/******************************************************************************
** File: network_includes.h
**
** Purpose:
**   This header file contains the correct set of network include
**   files, which are dependant on the operating system.
**
** Author:   R.McGraw/SSI
**
** Notes:
**
******************************************************************************/

#ifndef _network_includes_
#define _network_includes_

#ifdef _VXWORKS_OS_
    #include <vxWorks.h>
    #include <selectLib.h>
    #include <sockLib.h>
    #include <inetLib.h>
    #include <arpLib.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>

#elif  _RTEMS_OS_
    #define _USING_RTEMS_INCLUDES_
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <fcntl.h>
    #define _HAVE_FCNTL_
    #ifndef MSG_DONTWAIT
      #define MSG_DONTWAIT 0
    #endif
#elif  _MAC_OS_
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <fcntl.h>
    #define _HAVE_FCNTL_

#elif  _LINUX_OS_
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <fcntl.h>
    #define _HAVE_FCNTL_
    #ifndef MSG_DONTWAIT
      #define MSG_DONTWAIT 0
    #endif

#else
    #error "No OS defined!"
#endif

#endif /* _network_includes_ */
