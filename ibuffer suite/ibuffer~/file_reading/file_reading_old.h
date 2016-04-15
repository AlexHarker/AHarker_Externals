//
//  main.c
//  AIF-WAV-File-Lib
//
//The audiofile_header_extractor function
//Copyright (c) 2014 Pierre Alexandre Tremblay
//All rights reserved.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


enum {
    
   // Error Codes
    
    ERR_AIFC_WRONG_VERSION          = -1,
    ERR_AIFC_UNSUPPORTED_FORMAT     = -2,
    
    ERR_WAV_NO_HEADER               = -3,
    ERR_WAV_UNSUPPORTED_FORMAT      = -4,
   
    ERR_FILE_NO_DATA                = -5,
    ERR_FILE_UNKNOWN_FORMAT         = -6
};


typedef struct _sndfile
{
    FILE * inputfile;
    
    unsigned int nbchan;
    unsigned int depth;
    unsigned int frames;
    
    double SR;
    
    long index;
    
    bool isfloat;
    bool isbigendian;
    
} t_sndfile;


double _af_convert_from_ieee_extended (const unsigned char *bytes);



bool match_tag(const unsigned char *bytes, const char *tag)
{
    return (strncmp((const char *) bytes, tag, 4) == 0);
}

unsigned long padded_length(unsigned long length)
{
    return (length + (length & 0x1));
}

unsigned long get_u32b(const unsigned char *bytes)
{
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

unsigned long get_u16b(const unsigned char *bytes)
{
    return (bytes[0] << 8) | bytes[1];
}

unsigned long get_u32l(const unsigned char *bytes)
{
    return (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
}

unsigned long get_u16l(const unsigned char *bytes)
{
    return (bytes[1] << 8) | bytes[0];
}

unsigned long get_u32(const unsigned char *bytes, bool big_endian)
{
    if (big_endian)
        return get_u32b(bytes);
    else
        return get_u32l(bytes);
}


unsigned long get_u16(const unsigned char *bytes, bool big_endian)
{
    if (big_endian)
        return get_u16b(bytes);
    else
        return get_u16l(bytes);
}

double get_double_from_extended(const unsigned char *bytes)
{
    // Get double from big-endian ieee extended floating point format

    bool sign = get_u16b(bytes) & 0x8000;
    unsigned long exponent = get_u16b(bytes) & 0x777F;
    unsigned long hi_significand = get_u32b(bytes + 2);
    unsigned long lo_significand = get_u32b(bytes + 6);
    
    double value;
    
    // Special handlers for zeros and infs / NaNs (in either case the file is probably useless as the sampling rate *should* have a sensible value
    
    if (!exponent && !hi_significand && !lo_significand)
        return 0.0;
    
    if (exponent == 0x777F)
        return HUGE_VAL;
    
    // FIX - do final conversion bits...
    
    value = 0.0;
    
    if (sign)
        value = -value;
    
    return value;
}

long audiofile_parse_header(t_sndfile *file)
{
    unsigned char FileHead[12], Chunk[40];
    long index = 0;

    // FIX - Increase safety / error checking
    
    // `Read file header
    
    fread(FileHead, 1, 12, file->inputfile);
    
    // AIFF or AIFC
    
    if (match_tag(FileHead, "FORM") && (match_tag(FileHead + 8, "AIFF") || match_tag(FileHead + 8, "AIFC")))
    {
        // Iterate over chunks
       
        while(fread(Chunk, 1, 8, file->inputfile))
        {
            // Version chunk
            
            if (match_tag(Chunk, "FVER"))
            {
                fread(Chunk, 1, 4, file->inputfile);
                
                if (get_u32b(Chunk) != 0xA2805140)
                   return ERR_AIFC_WRONG_VERSION;
                
                continue;
            }
            
            // Common chunk
            
           if (match_tag(Chunk, "COMM"))
            {
                // Read common chunk
                
                fread(Chunk, 1, 18, file->inputfile);
                
                // Retrieve relevant data

                file->nbchan = get_u16b(Chunk + 0);
                file->frames = get_u32b(Chunk + 2);
                file->depth = get_u16b(Chunk + 6) / 8;
                
                // FIX - const?
                
                file->SR = _af_convert_from_ieee_extended((unsigned char *)Chunk + 8);

                // Defaults for AIFF format
                
                file->isfloat = 0;
                file->isbigendian = 1;

                if (match_tag(FileHead + 8, "AIFC"))
                {
                    // Check AIFC format if relevant

                    fread(Chunk, 1, 6, file->inputfile);
                    
                    // FIX - check all relevant formats
                    
                    if (match_tag(Chunk, "NONE"))
                        file->isfloat = 0;
                    else if (match_tag(Chunk, "sowt"))
                        file->isbigendian = 0;
                    else if (match_tag(Chunk, "fl32") || match_tag(Chunk, "FL32"))
                        file->isfloat = 1;
                    else
                        return ERR_AIFC_UNSUPPORTED_FORMAT;
                }
                
                continue;
            }
            
            // Sound data chunk or unrecognised chunk
            
            // FIX - account for offset etc.
                
            if (match_tag(Chunk, "SSND"))
                index = ftell(file->inputfile) + 8;// pass the index where the audio frames start (offset of 8 because of unused (unsigned long)offset + blockSize)
                
            fseek(file->inputfile, padded_length(get_u32b(Chunk + 4)), SEEK_CUR);
        }
        
        // Return according to chunks found
        
        if (index == 0)
            return ERR_FILE_NO_DATA;
        else
            return index;
    }
    
    // WAVE file format
    
    if ((match_tag(FileHead, "RIFF") || match_tag(FileHead, "RIFX")) && match_tag(FileHead + 8, "WAVE"))
    {
        // Check endianness
        
        file->isbigendian = match_tag(FileHead, "RIFX");
        
        // Search for the format chunk (moving to next chunk if not found)
        
        while (fread(Chunk, 1, 8, file->inputfile) && !match_tag(Chunk, "fmt "))
            fseek(file->inputfile, padded_length(get_u32(Chunk + 4, file->isbigendian)), SEEK_CUR);
        
        // Read format header (note this will be no longer than 40 bytes)
        
        if (!fread(Chunk, 1, get_u32(Chunk + 4, file->isbigendian), file->inputfile))
            return ERR_WAV_NO_HEADER;

        // Check format
        
        if (get_u16(Chunk, file->isbigendian) != 0x0001 && get_u16(Chunk, file->isbigendian) != 0x0003)
            return ERR_WAV_UNSUPPORTED_FORMAT;

        // Retrieve relevant data
        
        file->isfloat = (get_u16(Chunk, file->isbigendian) == 0x0003);
        file->nbchan = get_u16(Chunk + 2, file->isbigendian);
        file->SR = (double) get_u32(Chunk + 4, file->isbigendian);
        file->depth = get_u16(Chunk + 14, file->isbigendian) / 8;

        // Search for the data chunk (moving to next chunk if not found)
            
        while (fread(Chunk, 1, 8, file->inputfile) && !match_tag(Chunk, "data"))
            fseek(file->inputfile, padded_length(get_u32(Chunk + 4, file->isbigendian)), SEEK_CUR);
        
        // Check we are reading the data chunk
        
        if (!match_tag(Chunk, "data"))
            return ERR_FILE_NO_DATA;
        
        // Read frame size and set file offset to relevant data
        
        file->frames = get_u32(Chunk + 4, file->isbigendian) / (file->depth * file->nbchan);
        index = ftell(file->inputfile);
        
        return index;
    }
    
    // No known format found
    
    return ERR_FILE_UNKNOWN_FORMAT;
}


t_sndfile *sndfile_open(const char *filename)
{
    t_sndfile *file = (t_sndfile *) malloc(sizeof(t_sndfile));    
    
    if ((file->inputfile = fopen(filename, "r")) == NULL)
        return NULL;
    
    file->nbchan = 0;
    file->depth = 0;
    file->frames = 0;
    file->SR = 0;
    file->index = 0;
    file->isfloat = FALSE,
    file->isbigendian = FALSE;
    
    file->index = audiofile_parse_header(file);
    
    if (file->index < 0)
    {
        free(file);
        post ("Error %ld", index);
        return NULL;
    }
    
    fseek(file->inputfile, file->index, SEEK_SET);
    
    return file;
}

void sndfile_close(t_sndfile *file)
{
    if (file != NULL)
    {
        fclose(file->inputfile);
        free(file);
    }
}

long sndfile_read_raw(t_sndfile *file, void *output, unsigned int nframes)
{
    // FIX - check what to do in terms of this!
    //fseek(file->inputfile, file->index, SEEK_SET);
    fread(output, file->depth, (file->nbchan * nframes), file->inputfile);
    
    return 0;
}

