/***************************************************************************
 *
 *  Copyright (c) Microsoft Corporation.  All rights reserved.
 *
 *  File:     audiodefs.h
 *  Content:  Basic constants and data types for audio work.
 *
 *  Remarks:  This header file defines all of the audio format constants and
 *            structures required for XAudio2 and XACT work.  Providing these
 *            in a single location avoids certain dependency problems in the
 *            legacy audio headers (mmreg.h, mmsystem.h, ksmedia.h).
 *
 *            NOTE: Including the legacy headers after this one may cause a
 *            compilation error, because they define some of the same types
 *            defined here without preprocessor guards to avoid multiple
 *            definitions.  If a source file needs one of the old headers,
 *            it must include it before including audiodefs.h.
 *
 ***************************************************************************/

#ifndef __AUDIODEFS_INCLUDED__
#define __AUDIODEFS_INCLUDED__

#include <windef.h>  // For WORD, DWORD, etc.

#pragma pack(push, 1)  // Pack structures to 1-byte boundaries


/**************************************************************************
 *
 *  WAVEFORMATEX: Base structure for many audio formats.  Format-specific
 *  extensions can be defined for particular formats by using a non-zero
 *  cbSize value and adding extra fields to the end of this structure.
 *
 ***************************************************************************/

#ifndef _WAVEFORMATEX_

    #define _WAVEFORMATEX_
    typedef struct tWAVEFORMATEX
    {
        WORD wFormatTag;        // Integer identifier of the format
        WORD nChannels;         // Number of audio channels
        DWORD nSamplesPerSec;   // Audio sample rate
        DWORD nAvgBytesPerSec;  // Bytes per second (possibly approximate)
        WORD nBlockAlign;       // Size in bytes of a sample block (all channels)
        WORD wBitsPerSample;    // Size in bits of a single per-channel sample
        WORD cbSize;            // Bytes of extra data appended to this struct
    } WAVEFORMATEX;

#endif

// Defining pointer types outside of the #if block to make sure they are
// defined even if mmreg.h or mmsystem.h is #included before this file

typedef WAVEFORMATEX *PWAVEFORMATEX, *NPWAVEFORMATEX, *LPWAVEFORMATEX;
typedef const WAVEFORMATEX *PCWAVEFORMATEX, *LPCWAVEFORMATEX;


/**************************************************************************
 *
 *  WAVEFORMATEXTENSIBLE: Extended version of WAVEFORMATEX that should be
 *  used as a basis for all new audio formats.  The format tag is replaced
 *  with a GUID, allowing new formats to be defined without registering a
 *  format tag with Microsoft.  There are also new fields that can be used
 *  to specify the spatial positions for each channel and the bit packing
 *  used for wide samples (e.g. 24-bit PCM samples in 32-bit containers).
 *
 ***************************************************************************/

#ifndef _WAVEFORMATEXTENSIBLE_

    #define _WAVEFORMATEXTENSIBLE_
    typedef struct
    {
        WAVEFORMATEX Format;          // Base WAVEFORMATEX data
        union
        {
            WORD wValidBitsPerSample; // Valid bits in each sample container
            WORD wSamplesPerBlock;    // Samples per block of audio data; valid
                                      // if wBitsPerSample=0 (but rarely used).
            WORD wReserved;           // Zero if neither case above applies.
        } Samples;
        DWORD dwChannelMask;          // Positions of the audio channels
        GUID SubFormat;               // Format identifier GUID
    } WAVEFORMATEXTENSIBLE;

#endif

typedef WAVEFORMATEXTENSIBLE *PWAVEFORMATEXTENSIBLE, *LPWAVEFORMATEXTENSIBLE;
typedef const WAVEFORMATEXTENSIBLE *PCWAVEFORMATEXTENSIBLE, *LPCWAVEFORMATEXTENSIBLE;



/**************************************************************************
 *
 *  Define the most common wave format tags used in WAVEFORMATEX formats.
 *
 ***************************************************************************/

#ifndef WAVE_FORMAT_PCM  // Pulse Code Modulation

    // If WAVE_FORMAT_PCM is not defined, we need to define some legacy types
    // for compatibility with the Windows mmreg.h / mmsystem.h header files.

    // Old general format structure (information common to all formats)
    typedef struct waveformat_tag
    {
        WORD wFormatTag;
        WORD nChannels;
        DWORD nSamplesPerSec;
        DWORD nAvgBytesPerSec;
        WORD nBlockAlign;
    } WAVEFORMAT, *PWAVEFORMAT, NEAR *NPWAVEFORMAT, FAR *LPWAVEFORMAT;

    // Specific format structure for PCM data
    typedef struct pcmwaveformat_tag
    {
        WAVEFORMAT wf;
        WORD wBitsPerSample;
    } PCMWAVEFORMAT, *PPCMWAVEFORMAT, NEAR *NPPCMWAVEFORMAT, FAR *LPPCMWAVEFORMAT;

    #define WAVE_FORMAT_PCM 0x0001

#endif

// Other frequently used format tags

#ifndef WAVE_FORMAT_UNKNOWN
    #define WAVE_FORMAT_UNKNOWN         0x0000 // Unknown or invalid format tag
#endif

#ifndef WAVE_FORMAT_IEEE_FLOAT
    #define WAVE_FORMAT_IEEE_FLOAT      0x0003 // 32-bit floating-point
#endif

#ifndef WAVE_FORMAT_MPEGLAYER3
    #define WAVE_FORMAT_MPEGLAYER3      0x0055 // ISO/MPEG Layer3
#endif

#ifndef WAVE_FORMAT_DOLBY_AC3_SPDIF
    #define WAVE_FORMAT_DOLBY_AC3_SPDIF 0x0092 // Dolby Audio Codec 3 over S/PDIF
#endif

#ifndef WAVE_FORMAT_WMAUDIO2
    #define WAVE_FORMAT_WMAUDIO2        0x0161 // Windows Media Audio
#endif

#ifndef WAVE_FORMAT_WMAUDIO3
    #define WAVE_FORMAT_WMAUDIO3        0x0162 // Windows Media Audio Pro
#endif

#ifndef WAVE_FORMAT_WMASPDIF
    #define WAVE_FORMAT_WMASPDIF        0x0164 // Windows Media Audio over S/PDIF
#endif

#ifndef WAVE_FORMAT_EXTENSIBLE
    #define WAVE_FORMAT_EXTENSIBLE      0xFFFE // All WAVEFORMATEXTENSIBLE formats
#endif


/**************************************************************************
 *
 *  Define the most common wave format GUIDs used in WAVEFORMATEXTENSIBLE
 *  formats.  Note that including the Windows ksmedia.h header after this
 *  one will cause build problems; this cannot be avoided, since ksmedia.h
 *  defines these macros without preprocessor guards.
 *
 ***************************************************************************/
#pragma pack(pop)

#endif // #ifndef __AUDIODEFS_INCLUDED__
