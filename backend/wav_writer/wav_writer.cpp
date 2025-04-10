#include "wav_writer.hpp"
#include "../synthetiser/constants.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <sys/stat.h>

const int bit_depth = 16;

void writeToFile(std::ofstream &file, int value, int size) {
  file.write(reinterpret_cast<const char *>(&value), size);
}

int16_t convertFloatToAudioSample(float value) {
  double double_value = static_cast<double>(value);
  double_value *= 32'767.0f;
  if (double_value < -32'767.0f) {
    double_value = -32'767.0f;
  } else if (double_value > 32'767.0f) {
    double_value = 32'767.0f;
  }
  return static_cast<int16_t>(double_value);
}

std::optional<float> readFromFile(std::ifstream &file) {
  if (file.eof()) {
    return std::nullopt;
  }
  float float_chars;
  file.read(reinterpret_cast<char *>(&float_chars), sizeof(float));
  return float_chars;
}

bool fileExists(const std::string &filename) {
  struct stat buffer;
  return (stat(filename.c_str(), &buffer) == 0);
}

void WavWriter::writeWav() {
  std::string audiofile_name = "./recordings/recording.wav";
  size_t wav_file_number = 0;
  while (fileExists(audiofile_name)) {
    audiofile_name =
        "./recordings/recording" + std::to_string(++wav_file_number) + ".wav";
  }

  std::ofstream audiofile{audiofile_name};

  // https://docs.fileformat.com/audio/wav/
  // header chunk
  audiofile << "RIFF";
  audiofile << "----"; // placeholder for size
  audiofile << "WAVE";

  // Format Chunk
  audiofile << "fmt ";
  writeToFile(audiofile, bit_depth, 4);                   // size
  writeToFile(audiofile, 1, 2);                           // compression
  writeToFile(audiofile, 1, 2);                           // Number of channels
  writeToFile(audiofile, SAMPLE_RATE, 4);                 // sample rate
  writeToFile(audiofile, SAMPLE_RATE * bit_depth / 8, 4); // byte rate
  writeToFile(audiofile, bit_depth / 8, 2);               // block align
  writeToFile(audiofile, bit_depth, 2);                   // Bit depth

  // data chunk
  audiofile << "data";
  audiofile << "----"; // placeholder for size

  int preAudioPosition = audiofile.tellp();

  {
    std::ifstream temp_data{".tmp_recording"};
    std::optional<float> temp = 0.0f;
    while (true) {
      temp = readFromFile(temp_data);
      if (!temp.has_value()) {
        break;
      }
      writeToFile(audiofile, convertFloatToAudioSample(temp.value()),
                  bit_depth / 8);
    }
  }

  // clean the temp file
  std::remove(".tmp_recording");

  int postAudioPosition = audiofile.tellp();

  audiofile.seekp(preAudioPosition - 4);
  writeToFile(audiofile, postAudioPosition - preAudioPosition, 4);

  audiofile.seekp(4, std::ios::beg);
  writeToFile(audiofile, postAudioPosition - 8, 4);
}
