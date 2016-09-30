// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2016, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 vlcdemo.c
// Author:  suchao.wang
// Created: Mar 28, 2016
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <vlc/vlc.h>

 int main(int argc, char* argv[])
 {
     libvlc_instance_t * inst;
     libvlc_media_player_t *mp;
     libvlc_media_t *m;
     libvlc_exception_t ex;

     /* Load the VLC engine */
     inst = libvlc_new (0, NULL,NULL);

     /* Create a new item */
     m = libvlc_media_new (inst, "/media/sf_shared/720p.mp4",NULL);
     //m = libvlc_media_new_path (inst, "/path/to/test.mov");

     /* Create a media player playing environement */
     mp = libvlc_media_player_new_from_media (m,NULL);

     /* No need to keep the media now */
     libvlc_media_release (m);

 #if 0
     /* This is a non working code that show how to hooks into a window,
      * if we have a window around */
      libvlc_media_player_set_xwindow (mp, xid);
     /* or on windows */
      libvlc_media_player_set_hwnd (mp, hwnd);
     /* or on mac os */
      libvlc_media_player_set_nsobject (mp, view);
  #endif

     /* play the media_player */
     libvlc_media_player_play (mp,NULL);

     sleep (10); /* Let it play a bit */

     /* Stop playing */
     libvlc_media_player_stop (mp,NULL);

     /* Free the media_player */
     libvlc_media_player_release (mp);

     libvlc_release (inst);

     return 0;
 }



