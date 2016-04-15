
#ifndef FILE_READING
#define FILE_READING

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Error Codes

typedef enum {
    
    ERR_NONE                        =  0,

    ERR_MEM_COULD_NOT_ALLOCATE      = -1,

    ERR_FILE_ERROR                  = -2,
    ERR_FILE_COULDNT_OPEN           = -3,
    ERR_FILE_BAD_FORMAT             = -4,
    ERR_FILE_UNKNOWN_FORMAT         = -5,
    
    ERR_AIFC_WRONG_VERSION          = -6,
    ERR_AIFC_UNSUPPORTED_FORMAT     = -7,
    
    ERR_WAVE_UNSUPPORTED_FORMAT     = -8,
    
} t_sndfile_error;

// Endianness

typedef enum {
    
    ENDIANNESS_LITTLE                = 0,
    ENDIANNESS_BIG                   = 1,
    
    
} t_sndfile_endianness;

// Numerical format (float or int)

typedef enum {
    
    NUMFORMAT_INT                   = 0,
    NUMFORMAT_FLOAT                 = 1,
    
} t_sndfile_num_format;

// Info structure

// FIX - depth is currently in bytes - are we happy with this??
// FIX - Paranoia might dictate that we do checks on the lengths of audio data

// Types

// FIX - check this for windows

typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long      uint64_t;

typedef signed char             int8_t;
typedef short                   int16_t;
typedef signed int              int32_t;
typedef signed long long        int64_t;


typedef struct _sndfile_info
{
    uint32_t            depth;
    uint32_t            channels;
    size_t              frames;
    
    double                  sample_rate;
    
    t_sndfile_num_format    num_format;
    t_sndfile_endianness    endianness;
    
} t_sndfile_info;

// File structure (opaque to user)

typedef struct _sndfile
{
    FILE * inputfile;

    size_t data_offset;
    
    t_sndfile_info info;
    
} t_sndfile;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static uint32_t get_u32b(const unsigned char *bytes)
{
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

static uint32_t get_u24b(const unsigned char *bytes)
{
    return (bytes[0] << 16) | (bytes[1] << 8) | bytes[2];
}

static uint32_t get_u16b(const unsigned char *bytes)
{
    return (bytes[0] << 8) | bytes[1];
}

static uint32_t get_u32l(const unsigned char *bytes)
{
    return (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
}

static uint32_t get_u24l(const unsigned char *bytes)
{
    return (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
}

static uint32_t get_u16l(const unsigned char *bytes)
{
    return (bytes[1] << 8) | bytes[0];
}

static uint32_t get_u32(const unsigned char *bytes, t_sndfile_endianness endianness)
{
    if (endianness == ENDIANNESS_BIG)
        return get_u32b(bytes);
    else
        return get_u32l(bytes);
}

static uint32_t get_u24(const unsigned char *bytes, t_sndfile_endianness endianness)
{
    if (endianness == ENDIANNESS_BIG)
        return get_u24b(bytes);
    else
        return get_u24l(bytes);
}

static uint32_t get_u16(const unsigned char *bytes, t_sndfile_endianness endianness)
{
    if (endianness == ENDIANNESS_BIG)
        return get_u16b(bytes);
    else
        return get_u16l(bytes);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static double get_double_from_extended(const unsigned char *bytes)
{
    // Get double from big-endian ieee extended floating point format

    bool        sign            = get_u16b(bytes) & 0x8000;
    int32_t     exponent        = get_u16b(bytes) & 0x777F;
    uint32_t    hi_significand  = get_u32b(bytes + 2);
    uint32_t    lo_significand  = get_u32b(bytes + 6);
    
    double value;
    
    // Special handlers for zeros and infs / NaNs (in either case the file is probably useless as the sampling rate *should* have a sensible value)
    
    if (!exponent && !hi_significand && !lo_significand)
        return 0.0;
    
    if (exponent == 0x777F)
        return HUGE_VAL;
    
    exponent -= 16383;
    value  = ldexp((double) hi_significand, exponent - 31);
    value += ldexp((double) lo_significand, exponent - (31 + 32));
    
    if (sign)
        value = -value;
    
    return value;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static bool match_tag(const char *tag, const char *comparison_tag)
{
    return (strncmp(tag, comparison_tag, 4) == 0);
}

static size_t padded_length(size_t length)
{
    return (length + (length & 0x1));
}

static bool read_chunk_header(t_sndfile *file, char *tag, size_t *chunk_size, t_sndfile_endianness endianness)
{
    unsigned char header[8];
    
    if (fread(header, 1, 8, file->inputfile) != 8)
        return true;
    
    strncpy(tag, (char *) header, 4);
    *chunk_size = get_u32(header + 4, endianness);

    return false;
}

static bool find_chunk(t_sndfile *file, const char *search_tag, size_t *chunk_size)
{
    char tag[4];
    
    while (!read_chunk_header(file, tag, chunk_size, file->info.endianness))
    {
        if (match_tag(tag, search_tag))
            return false;
        
        if (fseek(file->inputfile, padded_length(*chunk_size), SEEK_CUR))
            return true;
    }
    
    return true;
}

static bool read_chunk(t_sndfile *file, unsigned char *data, size_t read_size, size_t chunk_size)
{
    if (read_size)
        if (read_size > chunk_size || fread(data, 1, read_size, file->inputfile) != read_size)
            return true;
    
    if (fseek(file->inputfile, padded_length(chunk_size) - read_size, SEEK_CUR))
        return true;
    
    return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define AIFC_CURRENT_SPECIFICATION 0xA2805140

typedef enum
{
    TAG_AIFC_UNKNOWN     = 0x0,
    TAG_AIFC_VERSION     = 0x1,
    TAG_AIFC_COMMON      = 0x2,
    TAG_AIFC_AUDIO       = 0x4,

} t_aiff_tag;


static bool get_aiff_chunk_header(t_sndfile *file, t_aiff_tag *tag_return, size_t *chunk_size)
{
    char tag[4];
    
    if (read_chunk_header(file, tag, chunk_size, ENDIANNESS_BIG))
        return true;
    
    if (match_tag(tag, "FVER"))
    {
        *tag_return = TAG_AIFC_VERSION;
        return false;
    }

    if (match_tag(tag, "COMM"))
    {
        *tag_return = TAG_AIFC_COMMON;
        return false;
    }
    
    if (match_tag(tag, "SSND"))
    {
        *tag_return = TAG_AIFC_AUDIO;
        return false;
    }
    
    *tag_return = TAG_AIFC_UNKNOWN;
    return false;
}


typedef enum
{
    AIFC_COMPRESSION_UNKNOWN            = 0,
    AIFC_COMPRESSION_NONE               = 1,
    AIFC_COMPRESSION_LITTLE_ENDIAN      = 2,
    AIFC_COMPRESSION_FLOAT              = 3,
    
} t_aifc_compression;

static t_aifc_compression get_aifc_compression(const char *tag)
{
    // FIX - finish this work
    
    if (match_tag(tag, "NONE"))
        return AIFC_COMPRESSION_NONE;
    
    // FIX - this is fixed to 16 bit?? (and the next??)
    
    if (match_tag(tag, "twos"))
        return AIFC_COMPRESSION_NONE;
    
    if (match_tag(tag, "sowt"))
        return AIFC_COMPRESSION_LITTLE_ENDIAN;
    
    if (match_tag(tag, "fl32"))
        return AIFC_COMPRESSION_FLOAT;
    
    if (match_tag(tag, "FL32"))
        return AIFC_COMPRESSION_FLOAT;
    
    // FIX - reading of these formats is only supported raw right now...
    
    if (match_tag(tag, "fl64"))
        return AIFC_COMPRESSION_FLOAT;
    
    if (match_tag(tag, "FL64"))
        return AIFC_COMPRESSION_FLOAT;

    return AIFC_COMPRESSION_UNKNOWN;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static t_sndfile_error audiofile_parse_header(t_sndfile *file, const char *file_name)
{
    char file_type[4], file_subtype[4];
    unsigned char chunk[22];
    size_t chunk_size;
    
    // Open file
    
    if ((file->inputfile = fopen(file_name, "r")) == NULL)
        return ERR_FILE_COULDNT_OPEN;

    // `Read file header
    
    if (!fread(chunk, 1, 12, file->inputfile))
        return ERR_FILE_BAD_FORMAT;
    
    // Get file type and subtype
    
    strncpy(file_type, (const char *) chunk, 4);
    strncpy(file_subtype, (const char *) chunk + 8, 4);
    
    // AIFF or AIFC
    
    if (match_tag(file_type, "FORM") && (match_tag(file_subtype, "AIFF") || match_tag(file_subtype, "AIFC")))
    {
        uint32_t format_valid = TAG_AIFC_COMMON | TAG_AIFC_AUDIO;
        uint32_t format_check = 0;
       
        t_aiff_tag tag;
        
        // Iterate over chunks
       
        while(!get_aiff_chunk_header(file, &tag, &chunk_size))
        {
            format_check |= tag;
            
            switch(tag)
            {
                case TAG_AIFC_VERSION:
                
                    // Read format number
                    
                    if (read_chunk(file, chunk, 4, chunk_size))
                        return ERR_FILE_BAD_FORMAT;

                    // Check for the correct version of the AIFC specification

                    if (get_u32b(chunk) != AIFC_CURRENT_SPECIFICATION)
                        return ERR_AIFC_WRONG_VERSION;
                    
                    break;
                    
                case TAG_AIFC_COMMON:
                    
                    // Read common chunk (at least 18 bytes and up to 22)
                
                    if (read_chunk(file, chunk, (chunk_size > 22) ? 22 : ((chunk_size < 18) ? 18 : chunk_size), chunk_size))
                        return ERR_FILE_BAD_FORMAT;
                    
                    // Retrieve relevant data (AIFF or AIFC) and set AIFF defaults

                    file->info.channels     = get_u16b(chunk + 0);
                    file->info.frames       = get_u32b(chunk + 2);
                    file->info.depth        = get_u16b(chunk + 6) / 8;
                    file->info.sample_rate  = get_double_from_extended(chunk + 8);
                    
                    file->info.num_format   = NUMFORMAT_INT;
                    file->info.endianness   = ENDIANNESS_BIG;
                    
                    // If there are no frames then it is not required for there to be an audio (SSND) chunk
                    
                    if (!file->info.frames)
                        format_check |= TAG_AIFC_AUDIO;
                    
                    if (match_tag(file_subtype, "AIFC"))
                    {
                        // Require a version chunk
                        
                        format_valid |= TAG_AIFC_VERSION;
                     
                        // Set parameters based on format
                        
                        switch (get_aifc_compression((const char *) (chunk + 18)))
                        {
                            case AIFC_COMPRESSION_NONE:
                                break;
                            
                            case AIFC_COMPRESSION_LITTLE_ENDIAN:
                                file->info.endianness = ENDIANNESS_LITTLE;
                                break;
                                
                            case AIFC_COMPRESSION_FLOAT:
                                file->info.num_format = NUMFORMAT_FLOAT;
                                break;
                            
                            default:
                                return ERR_AIFC_UNSUPPORTED_FORMAT;
                        }
                    }
                    
                    break;
                   
                case TAG_AIFC_AUDIO:
                    
                    // Audio data starts 8 bytes after this point in the file (2 x 32-bit values) + offset dealt with below
                    
                    file->data_offset = ftell(file->inputfile) + 8;
                    
                    if (read_chunk(file, chunk, 4, chunk_size))
                        return ERR_FILE_BAD_FORMAT;
                    
                    // Account for offset value (ignore block size value that comes after that)
                    
                    file->data_offset += get_u32b(chunk);
                    
                    break;
                    
                case TAG_AIFC_UNKNOWN:
                    
                    // Read no data, but update the file position
                    
                    if (read_chunk(file, NULL, 0, chunk_size))
                        return ERR_FILE_BAD_FORMAT;
                    
                    break;
            }
        }
        
        // Check that all relevant chunks were found
        
        if (format_check != format_valid)
            return ERR_FILE_BAD_FORMAT;
 
        return ERR_NONE;
    }
    
    // WAVE file format
    
    if ((match_tag(file_type, "RIFF") || match_tag(file_type, "RIFX")) && match_tag(file_subtype, "WAVE"))
    {
        // Check endianness
        
        file->info.endianness = match_tag(file_type, "RIFX") ? ENDIANNESS_BIG : ENDIANNESS_LITTLE;
        
        // Search for the format chunk and read first 16 bytes (ignored any extended header info)
        
        if (find_chunk(file, "fmt ", &chunk_size) || read_chunk(file, chunk, 16, chunk_size))
            return ERR_FILE_BAD_FORMAT;
        
        // Check for supported formats
        
        if (get_u16(chunk, file->info.endianness) != 0x0001 && get_u16(chunk, file->info.endianness) != 0x0003)
            return ERR_WAVE_UNSUPPORTED_FORMAT;

        // Retrieve relevant data
        
        file->info.num_format   = get_u16(chunk +  0, file->info.endianness) == 0x0003 ? NUMFORMAT_FLOAT : NUMFORMAT_INT;
        file->info.channels     = get_u16(chunk +  2, file->info.endianness);
        file->info.sample_rate  = get_u32(chunk +  4, file->info.endianness);
        file->info.depth        = get_u16(chunk + 14, file->info.endianness) / 8;

        // Search for the data chunk and retrieve frame size and file offset to audio data
        
        if (find_chunk(file, "data", &chunk_size))
            return ERR_FILE_BAD_FORMAT;
        
        file->info.frames = chunk_size / (file->info.depth * file->info.channels);
        file->data_offset = ftell(file->inputfile);
        
        return ERR_NONE;
    }
    
    // No known format found
    
    return ERR_FILE_UNKNOWN_FORMAT;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static t_sndfile_info sndfile_info(t_sndfile *file)
{
    return file->info;
}

static void sndfile_close(t_sndfile *file)
{
    // Free memory and close file
    
    if (file != NULL)
    {
        fclose(file->inputfile);
        free(file);
    }
}

static t_sndfile *sndfile_open(const char *file_name, t_sndfile_info *info, t_sndfile_error *error_return)
{
    t_sndfile *file = (t_sndfile *) malloc(sizeof(t_sndfile));

    t_sndfile_error error = ERR_NONE;
    
    if (!file)
        error = ERR_MEM_COULD_NOT_ALLOCATE;
    else
    {
        // Open and parse header
        
        error = audiofile_parse_header(file, file_name);
        
        // Seek the audio data if there has been no error
                                       
        if (!error && fseek(file->inputfile, file->data_offset, SEEK_SET))
            error = ERR_FILE_ERROR;
    }
    
    // Free memory and close file if there has been an error
    
    if (file && error)
    {
        sndfile_close(file);
        file = NULL;
    }
    
    // Return info if requested
    
    if (file && info)
        *info = sndfile_info(file);
    
    // Return error if requested
    
    if (error_return)
        *error_return = error;
    
    return file;
}

static inline t_sndfile_error sndfile_seek(t_sndfile *file, size_t frames)
{
    if (fseek(file->inputfile, file->data_offset + (file->info.channels * file->info.depth * frames), SEEK_SET))
        return ERR_FILE_ERROR;
    
    return ERR_NONE;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifdef _APPLE_
#define TWO_POW_31_RECIP			0x1.0fp-31f
#define TWO_POW_31_RECIP_DOUBLE		0x1.0fp-31
#else
#define TWO_POW_31_RECIP			0.000000000465661287f
#define TWO_POW_31_RECIP_DOUBLE		0.00000000046566128730773925
#endif

static float u32_to_float(uint32_t value)
{
    return *((int32_t *) & value) * TWO_POW_31_RECIP;
}

static float u32float_to_float(uint32_t value)
{
    return *((float *) &value);
}

static double u32_to_double(uint32_t value)
{
    return *((int32_t *) &value) * TWO_POW_31_RECIP_DOUBLE;
}

static double u32float_to_double(uint32_t value)
{
    return *((float *) &value);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static long sndfile_read_raw(t_sndfile *file, void *output, unsigned int nframes)
{
    // FIX - add EOF
    fread(output, file->info.depth, (file->info.channels * nframes), file->inputfile);
    
    return 0;
}


///

// FIX - cover more cases than this??

static long sndfile_read_double(t_sndfile *file, double *output, size_t nframes, long channel)
{
    // Read raw
    
    size_t output_frames = (channel < 0) ? file->info.channels * nframes : nframes;
    size_t byte_step = (channel < 0) ? file->info.depth: file->info.depth * file->info.channels;
    size_t j = (channel < 0) ? 0 : channel * file->info.depth;
    unsigned char *temp_buffer = (unsigned char *)malloc(file->info.depth * file->info.channels * nframes);

    if (!temp_buffer)
        return -1;
    
    sndfile_read_raw(file, temp_buffer, nframes);
    
    switch (file->info.depth)
    {
        case 1:
            for (size_t i = 0; i < output_frames; i++, j += byte_step)
                output[i] = u32_to_double(temp_buffer[j] << 24);
            break;
                
        case 2:
            for (size_t i = 0; i < output_frames; i++, j += byte_step)
                output[i] = u32_to_double(get_u16(temp_buffer + j, file->info.endianness) << 16);
            break;
                
        case 3:
            for (size_t i = 0; i < output_frames; i++, j += byte_step)
                output[i] = u32_to_double(get_u24(temp_buffer + j, file->info.endianness) << 8);
            break;
                
        case 4:
            if (file->info.num_format == NUMFORMAT_FLOAT)
                for (size_t i = 0; i < output_frames; i++, j += byte_step)
                    output[i] = u32float_to_double(get_u32(temp_buffer + j, file->info.endianness));
            else
                for (size_t i = 0; i < output_frames; i++, j += byte_step)
                    output[i] = u32_to_double(get_u32(temp_buffer + j, file->info.endianness));
            break;
    }
    
    free(temp_buffer);
    
    return 0;
}

static inline long sndfile_read_interleaved_double(t_sndfile *file, double *output, size_t nframes)
{
    return sndfile_read_double(file, output, nframes, -1);
}

static inline long sndfile_read_channel_double(t_sndfile *file, double *output, size_t nframes, unsigned int channel)
{
    return sndfile_read_double(file, output, nframes, channel);
}

static long sndfile_read_float(t_sndfile *file, float *output, size_t nframes, long channel)
{
    // Read raw
    
    size_t output_frames = (channel < 0) ? file->info.channels * nframes : nframes;
    size_t byte_step = (channel < 0) ? file->info.depth: file->info.depth * file->info.channels;
    size_t j = (channel < 0) ? 0 : channel * file->info.depth;
    unsigned char *temp_buffer = (unsigned char *)malloc(file->info.depth * file->info.channels * nframes);
    
    if (!temp_buffer)
        return -1;
    
    sndfile_read_raw(file, temp_buffer, nframes);
    
    switch (file->info.depth)
    {
        case 1:
            for (size_t i = 0; i < output_frames; i++, j += byte_step)
                output[i] = u32_to_float(temp_buffer[j] << 24);
            break;
            
        case 2:
            for (size_t i = 0; i < output_frames; i++, j += byte_step)
                output[i] = u32_to_float(get_u16(temp_buffer + j, file->info.endianness) << 16);
            break;
            
        case 3:
            for (size_t i = 0; i < output_frames; i++, j += byte_step)
                output[i] = u32_to_float(get_u24(temp_buffer + j, file->info.endianness) << 8);
            break;
            
        case 4:
            if (file->info.num_format == NUMFORMAT_FLOAT)
                for (size_t i = 0; i < output_frames; i++, j += byte_step)
                    output[i] = u32float_to_float(get_u32(temp_buffer + j, file->info.endianness));
            else
                for (size_t i = 0; i < output_frames; i++, j += byte_step)
                    output[i] = u32_to_float(get_u32(temp_buffer + j, file->info.endianness));
            break;
    }
    
    free(temp_buffer);
    
    return 0;
}

static inline long sndfile_read_interleaved_float(t_sndfile *file, float *output, size_t nframes)
{
    return sndfile_read_float(file, output, nframes, -1);
}

static inline long sndfile_read_channel_float(t_sndfile *file, float *output, size_t nframes, unsigned int channel)
{
    return sndfile_read_float(file, output, nframes, channel);
}


#endif