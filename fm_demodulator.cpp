#include <iostream>
#include <complex>

using namespace std;

const int32_t SAMPLE_RATE = 44100;

class FMD{
    float carrier_freq;
    float sample_rate;
    float time;
    float time_quanta;
    complex<float> previous_read;
public:
    FMD(float cf, float sr){
        carrier_freq = cf;
        sample_rate = sr;
        time_quanta = 1.0f / sample_rate;
        time = 0.0f;
        previous_read = {0.0f, 0.0f};
    }

    int32_t process(int32_t val) {
        float pcm_valf = float(val) / (float)((int32_t) 0x7fffffff);

        pcm_valf = pcm_valf * 2 * M_PI * carrier_freq * time * -1;
        complex<float> st = exp(complex<float>(0, pcm_valf));

        st = st * conj(previous_read);

        float angle = arg(st);

        previous_read = st;
        time = time + time_quanta;
        angle = angle / time_quanta;
        angle = angle * 10;
        int32_t ret = angle * ((int32_t) 0x7fffffff);
        return ret;
    }

};

int main(int argc, const char * argv[]) {

    FMD fmd(6000.0f, SAMPLE_RATE);

    int32_t audio_buffer[SAMPLE_RATE];
    int read_count;

    do{
        read_count = fread(audio_buffer, sizeof(int32_t), 44100, stdin);
        if(read_count == 0) break;
        for(int i = 0; i < read_count; i++) {
            int32_t sample = fmd.process(audio_buffer[i]);
            cout.write(reinterpret_cast<const char*>(&sample), 4);
        }
        
    }while(1);
    
    return 0;
}