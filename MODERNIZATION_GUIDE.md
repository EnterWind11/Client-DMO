# Guia de Modernização do Código Legacy

## 1. Modernização do C++

### 1.1. Smart Pointers
Substituir ponteiros brutos por smart pointers: 

```cpp
// Before
CREATE_SINGLETON(GameEventMng);
GameEventMng ptr = GAME_EVENT_STPTR;
// After
auto gameEventMng = std::make_unique<GameEventMng>();
```

### 1.2. RAII (Resource Acquisition Is Initialization)
Implementar classes RAII para recursos:

```cpp
class ScopedLock {
    CsCriticalSection& cs;
public:
    explicit ScopedLock(CsCriticalSection& cs) : cs(cs) { cs.Lock(); }
    ~ScopedLock() { cs.Unlock(); }
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
};
```

### 1.3. Tipos Modernos

```cpp
// Before
BOOL, LPCTSTR, LPCWSTR
// After
bool
std::string, std::string_view
std::wstring, std::wstring_view
```


## 2. Segurança e Thread Safety

### 2.1. Operações Atômicas
```cpp
// Before
CsCriticalSection g_GlobalCS;
int g_nGlobalState = 0;
// After
std::atomic<int> g_GlobalState{0};
```


### 2.2. Mutex e Locks
```cpp
// Before
g_GlobalCS.Lock();
g_nGlobalState = nState;
g_GlobalCS.Unlock();
// After
std::mutex g_GlobalMutex;
{
    std::lock_guard<std::mutex> lock(g_GlobalMutex);
    g_GlobalState = nState;
}
```


## 3. Gerenciamento de Recursos

### 3.1. Handles do Windows
```cpp
class ScopedHandle {
    HANDLE handle;
public:
    explicit ScopedHandle(HANDLE h) : handle(h) {}
    ~ScopedHandle() {
        if (handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
    }
    operator HANDLE() const { return handle; }
    HANDLE operator&() { return &handle; }
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
};
```


### 3.2. Internet Handles
```cpp
class ScopedInternetHandle {
    HINTERNET handle;
public:
    explicit ScopedInternetHandle(HINTERNET h) : handle(h) {}
    ~ScopedInternetHandle() {
        if (handle) InternetCloseHandle(handle);
    }
    operator HINTERNET() const { return handle; }
    ScopedInternetHandle(const ScopedInternetHandle&) = delete;
    ScopedInternetHandle& operator=(const ScopedInternetHandle&) = delete;
};
```


## 4. Tratamento de Erros

### 4.1. Sistema de Exceções
```cpp
class GameException : public std::runtime_error {
public:
    explicit GameException(const char msg) : std::runtime_error(msg) {}
};
class NetworkException : public GameException {
public:
    explicit NetworkException(const char msg) : GameException(msg) {}
};
```


### 4.2. Logging
```cpp
class Logger {
public:
    static void LogError(const std::string& message) {
        std::lock_guard<std::mutex> lock(logMutex);
        // Implement thread-safe logging
    }
    static void LogWarning(const std::string& message) {
        // ...
    }
private:
    static std::mutex logMutex;
};
```


## 5. Performance

### 5.1. String Views
```cpp
bool IsUiTexture(std::string_view name) {
    return name.starts_with("data\\Interface") &&
           std::filesystem::exists(std::string(name));
}


### 5.2. Move Semantics
std::string GetLargeString() {
    std::string result;
    // ... populate result ...
    return result; // Automatic move return
}
```


### 5.3. Otimização de Containers
```cpp
// Before
std::vector<GameObject> objects;
objects.push_back(new GameObject());
// After
std::vector<std::unique_ptr<GameObject>> objects;
objects.emplace_back(std::make_unique<GameObject>());


## 6. Boas Práticas

### 6.1. Constantes
// Before
#define CHECK_NPROTECT_TIME 5.0f
// After
static constexpr float CHECK_NPROTECT_TIME = 5.0f;


### 6.2. Enums Tipados
// Before
enum ProcessState { PROCESS_RUN, PROCESS_STOP };
// After
enum class ProcessState {
    Run,
    Stop
};


### 6.3. Inicialização Uniforme
// Before
RECT rect = { 0, 0, width, height };
// After
RECT rect{0, 0, width, height};
```

## 7. Próximos Passos

1. **Identificar Código Crítico**
   - Começar com partes mais utilizadas
   - Focar em áreas com problemas conhecidos

2. **Plano de Migração**
   - Criar testes unitários antes de modificar
   - Atualizar gradualmente
   - Manter compatibilidade backward

3. **Documentação**
   - Documentar mudanças
   - Atualizar guias de estilo
   - Manter registro de decisões arquiteturais

4. **Testes**
   - Implementar testes unitários
   - Adicionar testes de integração
   - Configurar CI/CD

## 8. Considerações de Compatibilidade

- Manter suporte a APIs antigas quando necessário
- Criar wrappers para código legacy
- Documentar dependências externas
- Verificar compatibilidade com diferentes versões do Windows
