# ClientBRDMO

## Overview
This is a client project developed in C++ using Visual Studio, consisting of multiple subprojects organized into different categories.

## Project Structure
The project is organized into the following main categories:

### CommonLib
- **CsFunc**: Common functions library
- **CsThread**: Thread management library
- **CsFilePack**: File packaging library
- **CsDM**: DM Module

### Network
- **common**: Common network library
- **nlib**: Network library

### Lib
- **CsGamebryo2.3**: Gamebryo engine integration
- **CsFileTable**: File table management
- **CsGBChar**: Character manipulation
- **lib_json**: JSON library

### Client
- **DProject**: Main client project

## Folder Structure
```plaintext
.
├── .gitattributes
├── .gitignore
├── ClientBRDMO.sln
├── DProject.sln
├── 3DMAX_PlugIn
│   ├── CMILoad
│   └── TerrainExportXML
├── build
│   ├── Lib
│   ├── obj
├── .idea
├── .codebuddy
```

## System Requirements
- Visual Studio 2022
- Windows (Win32)
- .NET Framework
- Basic knowledge of C++ and project structures

## Build Configurations
The project supports the following configurations:
- Debug
- Release
- Give
- Give_QA
- Give_English
- Give_English_QA
- Release_English

## Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/EnterWind11/Client-DMO.git
   ```
2. Open `ClientBRDMO.sln` or `DProject.sln` in Visual Studio
3. Build and run the project using the IDE's build tools

### Execution
- Use `Debug` or `Release` configuration as needed
- Refer to specific module documentation for more details

## Main Features
- Comprehensive plugins for 3D tools
- Modular structure for scalability
- Robust libraries for common operations

## Contributing
To contribute to the project:

1. Fork the repository
2. Create a branch for your feature:
   ```bash
   git checkout -b feature/your-feature
   ```
3. Commit your changes:
   ```bash
   git commit -m "Add your message here"
   ```
4. Push to your branch:
   ```bash
   git push origin feature/your-feature
   ```
5. Create a Pull Request

## Contact

For questions or support, contact:

- **Author**: Morthing

---

Feel free to modify this template according to your project's specific needs.

