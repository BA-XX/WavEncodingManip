# Audio Processing Toolkit

This project provides a set of tools for working with WAV audio files in C. It includes functionalities such as displaying hexadecimal code, reading and displaying WAV file headers, copying audio with mute in specified intervals, accelerating audio playback, generating data for GNU Plot, and visualizing audio waveforms.

## Features

- **Hexadecimal Code Display:** View the hexadecimal representation of the entire audio file.
  
- **WAV File Header Information:** Read and display essential information from the WAV file header, including format, channels, sample rate, and more.

- **Copy with Mute:** Create a new WAV file by muting audio within a specified time interval.

- **Audio Acceleration:** Speed up the audio playback in a WAV file by a specified acceleration factor.

- **GNU Plot Data Generation:** Generate data files compatible with GNU Plot for plotting audio waveforms.

- **Audio Waveform Visualization:** Use GNU Plot to visually represent the audio waveform.

## Getting Started

1. Clone the repository to your local machine.
2. Compile the code using a C compiler.
3. **Install GNU Plot:** Ensure GNU Plot is installed on your system. If not, download and install it.
4. **Update GNU Plot Path (if necessary):** If you have installed GNU Plot in a location other than `C:\gnuplot`, modify the `afficherFormeOndeAudio` function in the code to specify your installation path.

## Dependencies

- [GNU Plot]: Required for visualizing audio waveforms. Ensure it is installed, or update the path in the source code accordingly.

## Contributing

Contributions are welcome! Feel free to open issues, submit pull requests, or suggest new features.
