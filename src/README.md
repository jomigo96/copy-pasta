# Projeto PSis
### Autores
João Gonçalves 81040
Daniel de Schiffart 81479

### Conteúdo
 - clipboard.h
 - clipboard-dev.h
 - clipboard.c
 - clipboard_func.c
 - library.c
 - app_teste.c
 - app_teste2.c
 - Makefile

## Instalação
Para compilar todos os ficheiros usar: 
```
$ make
``` 
O clipboard é compilado com os ficheiros ```clipboard-dev.h clipboard.c``` e ```clipboard-func.c```. A biblioteca é gerada a partir dos ficheiros ``` library.c clipboard-dev.h``` e ``` clipboard.h```, e movida para a directoria ```/lib``` sob o nome ```libclipboard.so```. 

As aplicações devem usar a API contituída por ```clipboard.h``` e ligar dinamicamente ao ficheiro .so.

## Utilização

Correr cada clipboard numa directoria separada. A aplicação deve conhecer esta diretoria, e usar a função clipboard_connect, como exemplo:
```c
fd=clipboard_connect("./");
``` 
ou 
```c
fd=clipboard_connect("./dir/");
```

O clipboard pode ser executado como:
```
./clipboard
```
ou
```
./clipboard -c <IP> <port>
```
### app_teste

Este protótipo testa a maioria das funcionalidades, ligando-se ao clipboard presente na directoria onde é executado.

### app_teste2

Este programa liga-se a dois clipboards, presentes nas diretorias ```./dir1/``` e ```./dir2/``` e envia para estes simultaneamente duas mensagens diferentes, pedindo de volta as respostas, de modo a testar a sincronização global de uma árvore de clipboards.


## Desinstalação

Usar
```
$ make clean
```
para remover todos os ficheiros executáveis, assim como .o e .so. Não são criados ficheiros fora da pasta deste projeto.


###

O repositório [Github](https://github.com/jomigo96/copy-pasta) deste projeto. 
