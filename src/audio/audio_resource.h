#pragma once

#include "always.h"
#include "wstring.h"
#include "crc32.h"
#include "ccfile.h"
#include "asserthandler.h"


class AudioResource
{
    public:
        AudioResource(Wstring &filename, Wstring &name = Wstring("No Name")) :
            Filename(filename),
            Handle(INVALID_AUDIO_HANDLE),
            FileHandle(nullptr)
        {
            CCFileClass *handle = new CCFileClass(filename.Peek_Buffer());
            if (handle) {
                
                FileHandle = handle;
                FileHandle->Open(FILE_ACCESS_READ);

                if (FileHandle->Is_Available()) {

                    Handle = Generate_File_Handle(filename);

                }

            }
        }

        virtual ~AudioResource()
        {
            delete FileHandle;
            FileHandle = nullptr;
        }

        virtual const Wstring &Get_Filename() const { return Filename; }
        virtual bool Is_Available() const { ASSERT(FileHandle != nullptr); return FileHandle->Is_Available(); }
        virtual const UINT8 *Get_Audio_Data() const = 0;
        virtual UINT32 Get_Audio_Bytes() const = 0;

        uint32_t Get_Handle() const { return Handle; }

    protected:
        /**
         *  Generates a unique hash based off the filename.
         */
        static uint32_t Generate_File_Handle(const Wstring &filename)
        {
            return CRC32_Memory(filename.Peek_Buffer(), filename.Get_Length());
        }

        /**
         *  Generates a unique hash based off the first 128 bytes of the sound file.
         */
        static uint32_t Generate_Sample_Handle(AudioSample sample)
        {
            return CRC32_Memory(sample, 128);
        }

    protected:
        /**
         *  
         */
        Wstring Filename;

        /**
         *  
         */
        uint32_t Handle;

        /**
         *  
         */
        CCFileClass *FileHandle;
};






#include <vector>
#include "buff.h"
#include "asserthandler.h"

#define MAKE_WAVE_TAG_VALUE(c1, c2, c3, c4) (c1 | (c2<<8) | (c3<<16) | (c4<<24))


static class WaveAudioResource : public AudioResource
{
    public:
        WaveAudioResource(Wstring &filename) :
            AudioResource(filename)
        {
            FILE* fp = nullptr;
            errno_t error = fopen_s(&fp, filename.Peek_Buffer(), "rb");
            ASSERT_PRINT(error == 0, "WAV File not found!");

            fseek(fp, 0, SEEK_END);
            size_t size = static_cast<size_t>(ftell(fp));
            fseek(fp, 0, SEEK_SET);

            size_t read_bytes = 0;

            fread(&riff, sizeof(riff), 1, fp);
            read_bytes += sizeof(riff);

            ASSERT_PRINT(riff.tag == MAKE_WAVE_TAG_VALUE('R', 'I', 'F', 'F'), "not in RIFF format");

            ASSERT_PRINT(riff.type == MAKE_WAVE_TAG_VALUE('W', 'A', 'V', 'E'), "not in WAVE format");
            while (size > read_bytes)
            {
                Chunk chunk;
                fread(&chunk, sizeof(chunk), 1, fp);
                read_bytes += sizeof(chunk);

                if (chunk.tag == MAKE_WAVE_TAG_VALUE('f', 'm', 't', ' '))
                {
                    fread(&fmt, sizeof(fmt), 1, fp);
                    read_bytes += sizeof(fmt);

                    if (chunk.size > sizeof(Fmt))
                    {
                        UINT16 extSize;
                        fread(&extSize, sizeof(extSize), 1, fp);
                        read_bytes += sizeof(extSize);

                        if (read_bytes + chunk.size == size)
                        {
                            break;
                        }

                        fseek(fp, extSize, SEEK_CUR);
                        read_bytes += extSize;
                    }
                }
                else if (chunk.tag == MAKE_WAVE_TAG_VALUE('d', 'a', 't', 'a'))
                {
                    data.resize(chunk.size);
                    fread(data.data(), chunk.size, 1, fp);
                    read_bytes += chunk.size;

                    if (fmt.quantum_bits == 8)
                    {
                        for (UINT32 i = 0; i < chunk.size; ++i)
                        {
                            data[i] -= 128;
                        }
                    }
                }
                else
                {
                    if (read_bytes + chunk.size == size) break;

                    fseek(fp, chunk.size, SEEK_CUR);
                    read_bytes += chunk.size;
                }
            }

            fclose(fp);

            wfx.wFormatTag = WAVE_FORMAT_PCM;
            wfx.nChannels = 2;
            wfx.nSamplesPerSec = 44100;
            wfx.wBitsPerSample = 16;
            wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
            wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
            wfx.cbSize = sizeof(WAVEFORMATEX);
        }

        virtual ~WaveAudioResource()
        {
        }

        virtual const UINT8 *Get_Audio_Data() const override { return data.data(); }
        virtual UINT32 Get_Audio_Bytes() const override { return static_cast<UINT32>(data.size()); }

        const WAVEFORMATEX &GetWaveFormat() const { return wfx; }

    private:
        struct Riff
        {
            UINT32 tag;
            UINT32 size;
            UINT32 type;
        };

        struct Chunk
        {
            UINT32 tag;
            UINT32 size;
        };

        struct Fmt
        {
            UINT16 fmt_id;
            UINT16 channel;
            UINT32 sample_rate;
            UINT32 trans_rate;
            UINT16 block_size;
            UINT16 quantum_bits;
        };

        Riff riff;
        Fmt fmt;
        std::vector<UINT8> data; // Buffer data;
        WAVEFORMATEX wfx;
};




#include "dsaudio.h"
#include "wwaud.h"
#include "soscodec.h"

static class WestwoodAudioResource : public AudioResource
{
    public:
        WestwoodAudioResource(Wstring &filename) :
            AudioResource(filename)
        {
        }

        virtual ~WestwoodAudioResource()
        {
        }

    private:
        /**
         *  This is the compression that the sound data is using.
         */
        SCompressType Compression;
        
        /**
         *  This is a copy of the sample file header.
         */
        AUDHeaderType Header;
        
        /**
         *  The following structure is used if the sample if compressed using
         *  the sos 16 bit compression codec.
         */
        _tagCOMPRESS_INFO2 sSOSInfo;
};


