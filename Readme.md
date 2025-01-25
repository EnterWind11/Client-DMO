# ClientBRDMO

## Visão Geral
Este é um projeto cliente desenvolvido em C++ usando Visual Studio, consistindo em múltiplos subprojetos organizados em diferentes categorias.

## Estrutura do Projeto
O projeto está organizado nas seguintes categorias principais:

### CommonLib
- **CsFunc**: Biblioteca de funções comuns
- **CsThread**: Biblioteca para gerenciamento de threads
- **CsFilePack**: Biblioteca para empacotamento de arquivos
- **CsDM**: Módulo DM

### Network
- **common**: Biblioteca comum de rede
- **nlib**: Biblioteca de rede

### Lib
- **CsGamebryo2.3**: Integração com engine Gamebryo
- **CsFileTable**: Gerenciamento de tabelas de arquivos
- **CsGBChar**: Manipulação de caracteres
- **lib_json**: Biblioteca JSON

### Client
- **DProject**: Projeto principal do cliente

## Estrutura de Pastas
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

## Requisitos do Sistema
- Visual Studio 2022
- Windows (Win32)
- .NET Framework
- Conhecimento básico de C++ e estruturas de projeto

## Configurações de Build
O projeto suporta as seguintes configurações:
- Debug
- Release
- Give
- Give_QA
- Give_English
- Give_English_QA
- Release_English

## Instalação

1. Clone este repositório:
   ```bash
   git clone https://github.com/kleberrhuan/ClientBRDMO.git
   ```
2. Abra `ClientBRDMO.sln` ou `DProject.sln` no Visual Studio
3. Compile e execute o projeto usando as ferramentas de build do IDE

### Execução
- Use a configuração `Debug` ou `Release` conforme necessário
- Consulte a documentação específica de cada módulo para mais detalhes

## Recursos Principais
- Plugins abrangentes para ferramentas 3D
- Estrutura modular para escalabilidade
- Bibliotecas robustas para operações comuns

## Contribuição
Para contribuir com o projeto:

1. Faça fork do repositório
2. Crie um branch para sua feature:
   ```bash
   git checkout -b feature/sua-feature
   ```
3. Faça commit das alterações:
   ```bash
   git commit -m "Adicione sua mensagem aqui"
   ```
4. Faça push para seu branch:
   ```bash
   git push origin feature/sua-feature
   ```
5. Crie um Pull Request

## Contato

Para questões ou suporte, entre em contato:

- **Autor**: Morthing

---

Sinta-se à vontade para modificar este modelo de acordo com as necessidades específicas do seu projeto.

