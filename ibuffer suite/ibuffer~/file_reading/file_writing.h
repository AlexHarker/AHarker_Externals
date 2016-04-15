
#ifndef FILE_WRITING
#define FILE_WRITING

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Error Codes
/*
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


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define AIFC_CURRENT_SPECIFICATION 0xA2805140

typedef enum
{
    AIFC_COMPRESSION_UNKNOWN            = 0,
    AIFC_COMPRESSION_NONE               = 1,
    AIFC_COMPRESSION_LITTLE_ENDIAN      = 2,
    AIFC_COMPRESSION_FLOAT              = 3,
    
} t_aifc_compression;
*/

// File structure (opaque to user)

typedef enum {
    
    SNDFILE_TYPE_WAVE               = 0,
    SNDFILE_TYPE_AIFC               = 1,
    
} t_sndfile_type;

typedef struct _writefile
{
    FILE *file;
    
    t_sndfile_type type;
    
    size_t header_size;
    
    t_sndfile_info info;
    
} t_writefile;


const char *aifc_compression_tags[] = {"NONE", "sowt", "fl32", "fl64"};
const char *aifc_compression_strings[] = {"not compressed", "little endian", "32-bit floating point", "64-bit floating point"};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// FINISH REPLACING THIS! ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define FloatToUnsigned(f) ((unsigned long) (((long) (f - 2147483648.0)) + 2147483647L) + 1)

void _af_convert_to_ieee_extended (double num, unsigned char *bytes)
{
    int				sign;
    int				expon;
    double			fMant, fsMant;
    unsigned long	hiMant, loMant;
    
    if (num < 0) {
        sign = 0x8000;
        num *= -1;
    } else {
        sign = 0;
    }
    
    if (num == 0) {
        expon = 0; hiMant = 0; loMant = 0;
    }
    else {
        fMant = frexp(num, &expon);
        if ((expon > 16384) || !(fMant < 1)) {	  /* Infinity or NaN */
            expon = sign|0x7FFF; hiMant = 0; loMant = 0; /* infinity */
        }
        else {	  /* Finite */
            expon += 16382;
            if (expon < 0) {	/* denormalized */
                fMant = ldexp(fMant, expon);
                expon = 0;
            }
            expon |= sign;
            fMant = ldexp(fMant, 32);
            fsMant = floor(fMant);
            hiMant = FloatToUnsigned(fsMant);
            fMant = ldexp(fMant - fsMant, 32);
            fsMant = floor(fMant);
            loMant = FloatToUnsigned(fsMant);
        }
    }
    
    bytes[0] = expon >> 8;
    bytes[1] = expon;
    bytes[2] = hiMant >> 24;
    bytes[3] = hiMant >> 16;
    bytes[4] = hiMant >> 8;
    bytes[5] = hiMant;
    bytes[6] = loMant >> 24;
    bytes[7] = loMant >> 16;
    bytes[8] = loMant >> 8;
    bytes[9] = loMant;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Check - do bigendian cases...

bool write_u32b(FILE *outputfile, uint32_t value)
{
    unsigned char bytes[4];
    
    bytes[0] = (value >> 24) & 0xFF;
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >>  8) & 0xFF;
    bytes[3] = (value >>  0) & 0xFF;
    
    return fwrite(bytes, 1, 4, outputfile) != 4;
}

bool write_u24b(FILE *outputfile, uint32_t value)
{
    unsigned char bytes[3];
    
    bytes[0] = (value >> 16) & 0xFF;
    bytes[1] = (value >>  8) & 0xFF;
    bytes[2] = (value >>  0) & 0xFF;
    
    return fwrite(bytes, 1, 3, outputfile) != 3;
}

bool write_u16b(FILE *outputfile, uint32_t value)
{
    unsigned char bytes[2];
    
    bytes[0] = (value >>  8) & 0xFF;
    bytes[1] = (value >>  0) & 0xFF;
    
    return fwrite(bytes, 1, 2, outputfile) != 2;
}

bool write_u32l(FILE *outputfile, uint32_t value)
{
    unsigned char bytes[4];
    
    bytes[0] = (value >>  0) & 0xFF;
    bytes[1] = (value >>  8) & 0xFF;
    bytes[2] = (value >> 16) & 0xFF;
    bytes[3] = (value >> 24) & 0xFF;

    return fwrite(bytes, 1, 4, outputfile) != 4;
}

bool write_u24l(FILE *outputfile, uint32_t value)
{
    unsigned char bytes[3];
    
    bytes[0] = (value >>  0) & 0xFF;
    bytes[1] = (value >>  8) & 0xFF;
    bytes[2] = (value >> 16) & 0xFF;
    
    return fwrite(bytes, 1, 3, outputfile) != 3;
}

bool write_u16l(FILE *outputfile, uint32_t value)
{
    unsigned char bytes[2];
    
    bytes[0] = (value >>  0) & 0xFF;
    bytes[1] = (value >>  8) & 0xFF;
    
    return fwrite(bytes, 1, 2, outputfile) != 2;
}

bool write_u32(FILE *outputfile, uint32_t value, t_sndfile_endianness endianness)
{
    if (endianness == ENDIANNESS_BIG)
        return write_u32b(outputfile, value);
    else
        return write_u32l(outputfile, value);
}

bool write_u24(FILE *outputfile, uint32_t value, t_sndfile_endianness endianness)
{
    if (endianness == ENDIANNESS_BIG)
        return write_u24b(outputfile, value);
    else
        return write_u24l(outputfile, value);
}

bool write_u16(FILE *outputfile, uint32_t value, t_sndfile_endianness endianness)
{
    if (endianness == ENDIANNESS_BIG)
        return write_u16b(outputfile, value);
    else
        return write_u16l(outputfile, value);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool write_extended(FILE *outputfile, double value)
{
    unsigned char bytes[10];
    
    // FIX - this needs doing here by ME
    
    _af_convert_to_ieee_extended(value, bytes);
    
    return fwrite (bytes, 1, 10, outputfile) != 10;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool write_tag(FILE *outputfile, const char *tag)
{
    return fwrite (tag, 1, 4, outputfile) != 4;
}
/*
unsigned long padded_length(unsigned long length)
{
    return (length + (length & 0x1));
}*/

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool write_pcm_data(t_writefile *output_file, size_t frames)
{
    // N.B. Assumes writing to end of file...
    
    bool error = FALSE;
    char zero = 0;
    size_t num_bytes = (output_file->info.channels * output_file->info.depth * frames / 8);
    size_t data_bytes = num_bytes + (output_file->info.channels * output_file->info.depth * output_file->info.frames / 8);
    size_t data_end;
    
    // If there is no SSND chunk yet add one
    
    if (output_file->type == SNDFILE_TYPE_AIFC && output_file->info.frames == 0)
    {
        // SSND Chunk
    
        error |= write_tag(output_file->file, "SSND");
        error |= write_u32b(output_file->file, 8 + (frames * output_file->info.channels * output_file->info.depth / 8));
        error |= write_u32b(output_file->file, 0);
        error |= write_u32b(output_file->file, 0);
        
        output_file->header_size += 16;
    }
    
    // Write zeroed data for test purposes...
    
    for (unsigned long i = 0; i < num_bytes; i++)
        error |= fwrite(&zero, 1, 1, output_file->file) != 1;
       
    // Write padding bit if relevant (rewinding file by one byte for next write)
    
    if (data_bytes & 0x1)
    {
        error |= fwrite(&zero, 1, 1, output_file->file) != 1;
        fseek(output_file->file, -1, SEEK_CUR);
    }
    
    // Update number of frames
    
    output_file->info.frames += frames;
    
    // Update chunk size for file and audio data and frames for aifc and reset position to the end of the data
    
    data_end = ftell(output_file->file);
    
    if (output_file->type == SNDFILE_TYPE_WAVE)
    {
        fseek(output_file->file, 4, SEEK_SET);
        error |= write_u32(output_file->file, output_file->header_size + padded_length(data_bytes), output_file->info.endianness);
        fseek(output_file->file, 40, SEEK_SET);
        error |= write_u32(output_file->file, data_bytes, output_file->info.endianness);
    }
    else
    {
        fseek(output_file->file, 4, SEEK_SET);
        error |= write_u32b(output_file->file, output_file->header_size + padded_length(data_bytes));
        fseek(output_file->file, 36, SEEK_SET);
        error |= write_u32b(output_file->file, output_file->info.frames);
        fseek(output_file->file, output_file->header_size - 4, SEEK_SET);
        error |= write_u32b(output_file->file, data_bytes);
    }
    fseek(output_file->file, data_end, SEEK_SET);
    
    return error;
}

bool write_wave_header(t_writefile *output_file)
{
    bool error = FALSE;
    
    // Copy local variables
    
    FILE *file = output_file->file;
    t_sndfile_info info = output_file->info;
    
    // Set file type, data size offset frames and header size
    
    output_file->type = SNDFILE_TYPE_WAVE;
    output_file->header_size = 36;
    output_file->info.frames = 0;
    
    // File Header
    
    if (info.endianness == ENDIANNESS_LITTLE)
        error |= write_tag(file, "RIFF");
    else
        error |= write_tag(file, "RIFX");
    
    error |= write_u32(file, 36, info.endianness);
    error |= write_tag(file, "WAVE");
   
    // Format Chunk
    
    error |= write_tag(file, "fmt ");
    error |= write_u32(file, 16, info.endianness);                                                      // chunk size
    error |= write_u16(file, info.num_format == NUMFORMAT_INT ? 1: 3, info.endianness);                 // numeric format (fix with defines)
    error |= write_u16(file, info.channels, info.endianness);                                           // num channels
    error |= write_u32(file, info.sample_rate, info.endianness);                                        // sample rate
    error |= write_u32(file, info.sample_rate * info.depth * info.channels / 8, info.endianness);       // bytes per second
    error |= write_u16(file, info.depth * info.channels / 8, info.endianness);                          // block alignment
    error |= write_u16(file, info.depth, info.endianness);                                              // depth

    // Data Chunk (empty)
    
    error |= write_tag(file, "data");
    error |= write_u32(file, 0, info.endianness);

    return error;
}

int length_as_pstring(const char *string)
{
    int length = strlen(string);
    
    if (!(length & 0x1))
        return length + 2;
    
    return length + 1;
}

bool write_pstring (FILE *file, const char *string)
{
    bool error = FALSE;
    char zero = 0;
    char length = strlen(string);

    error |= fwrite (&length, 1, 1, file) != 1;
    error |= fwrite (string, 1, length, file) != length;
    
    // Pad bit if necessary
    
    if (!(length & 0x1))
        error |= fwrite (&zero, 1, 1, file) != 1;
    
    return error;
}

bool write_aifc_header(t_writefile *output_file)
{
    bool error = FALSE;
    
    // Copy local variables
    
    FILE *file = output_file->file;
    t_sndfile_info info = output_file->info;
    
    // FIX - Calculate compression
    
    t_aifc_compression compression = AIFC_COMPRESSION_NONE;
    
    // Set file type, data size offset frames and header size
    
    output_file->type = SNDFILE_TYPE_AIFC;
    output_file->header_size = 46 + length_as_pstring(aifc_compression_strings[compression]);
    output_file->info.frames = 0;
    
    // File Header
    
    error |= write_tag(file, "FORM");
    error |= write_u32b(file, output_file->header_size);
    error |= write_tag(file, "AIFC");
    
    // FVER Chunk
    
    error |= write_tag(file, "FVER");
    error |= write_u32b(file, 4);
    error |= write_u32b(file, AIFC_CURRENT_SPECIFICATION);
    
    // COMM Chunk
    
    error |= write_tag(file, "COMM");
    error |= write_u32b(file, 22 + length_as_pstring(aifc_compression_strings[compression]));
    
    error |= write_u16b(file, info.channels);                                       // num channels
    error |= write_u32b(file, 0);                                                   // num frames
    error |= write_u16b(file, info.depth);                                          // depth
    error |= write_extended(file, info.sample_rate);                                // sample rate
    error |= write_tag(file, aifc_compression_tags[compression]);                   // compression tag
    error |= write_pstring(file, aifc_compression_strings[compression]);            // compression string
    
    return error;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


t_sndfile_info writefile_info(t_writefile *file)
{
    return file->info;
}

void writefile_close(t_writefile *file)
{
    // Free memory and close file
    
    if (file != NULL)
    {
        fclose(file->file);
        free(file);
    }
}

// FIX - do sanity checks

t_writefile *open_wave(const char *file_name, unsigned long channels, unsigned long depth, double sample_rate, t_sndfile_num_format num_format, t_sndfile_endianness endianness)
{
    t_writefile *file = (t_writefile *) malloc(sizeof(t_writefile));
    
    t_sndfile_error error = ERR_NONE;
    
    if (!file)
        error = ERR_MEM_COULD_NOT_ALLOCATE;
    else
    {
        if ((file->file = fopen(file_name, "w")) == NULL)
            error = ERR_FILE_COULDNT_OPEN;
        
        file->type = SNDFILE_TYPE_WAVE;
        
        file->info.channels = channels;
        file->info.depth = depth;
        file->info.sample_rate = sample_rate;
        file->info.num_format = num_format;
        file->info.endianness = endianness;
        
        write_wave_header(file);
    }
    
    return file;
}

t_writefile *open_aifc(const char *file_name, unsigned long channels, unsigned long depth, unsigned long sample_rate, t_sndfile_num_format num_format, t_sndfile_endianness endianness)
{
    t_writefile *file = (t_writefile *) malloc(sizeof(t_writefile));
    
    t_sndfile_error error = ERR_NONE;
    
    if (!file)
        error = ERR_MEM_COULD_NOT_ALLOCATE;
    else
    {
        if ((file->file = fopen(file_name, "w")) == NULL)
            error = ERR_FILE_COULDNT_OPEN;
        
        file->type = SNDFILE_TYPE_AIFC;
        
        file->info.channels = channels;
        file->info.depth = depth;
        file->info.sample_rate = sample_rate;
        file->info.num_format = num_format;
        file->info.endianness = endianness;
        
        write_aifc_header(file);
    }

    return file;
}
#endif