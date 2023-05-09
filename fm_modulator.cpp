#include <iostream>
#include <complex>

using namespace std;

const int32_t SAMPLE_RATE = 44100;

class OS{
    float freq;
    float sample_rate;
    float time_qunata;
    float time;
public:
    OS(float f, float s) {
        freq = f;
        sample_rate = s;
        time = 0.0f;
        time_qunata = 1.0f / sample_rate;
    }
    int32_t sample() {
        float st = 2* M_PI* freq * time;
        st = sinf(st);
        time += time_qunata;
        int32_t ret = (int32_t)0x7fffffff * st;
        return ret;
    }
};

class FM{
    float carrier_amp;
    float carrier_freq;
    float carrier_sensitivity;
    float sample_rate;
    float time_qunata;
    float time;
    float input_intregator;
public:
    FM(float cf, float fs, float sr) {
        carrier_amp = 1.0f;
        carrier_freq = cf;
        carrier_sensitivity = fs;
        sample_rate = sr;
        time_qunata = 1.0f / sr;
        time = 0.0f;
        input_intregator = 0.0f;
    }

    int32_t process(int32_t pcm_val) {
        float pcm_valf = float(pcm_val) / (float)((int32_t) 0x7fffffff);

        input_intregator = pcm_valf;

        float st = input_intregator;

        st = 2 * M_PI * carrier_sensitivity * st;

        st = 2 * M_PI * carrier_freq * time + st;
        
        st = sinf(st);
        
        st = carrier_amp * st;
        
        time += time_qunata;

        if(time > 1.0f) {
            time = 1.0f - time;
        }
        
        int32_t ret = ((int32_t)0x7fffffff) * st;
        
        return ret;
    }
};

int main(int argc, const char * argv[]) {

    FM fm(5000.0f, 1.0f, SAMPLE_RATE);
    OS os(25.0f, SAMPLE_RATE);

    int32_t audio_buffer[SAMPLE_RATE];
    int read_count;

    do{
        read_count = fread(audio_buffer, sizeof(int32_t), 44100, stdin);
        if(read_count == 0) break;
        int cnt = 0;
        for(int i = 0; i < SAMPLE_RATE; i++) {
            int32_t sample = fm.process(audio_buffer[i]);
            cout.write(reinterpret_cast<const char*>(&sample), 4);
            cnt ++;
        }
        
    }while(1);
    
    return 0;
}