# Compressor e Descompressor Huffman

Projeto de Estrutura de Dados utilizando Árvore de Huffman em C++.

## Compilar

No Git Bash:

```bash
g++ compressor.cpp -o compressor.exe
g++ descompressor.cpp -o descompressor.exe
```

## Executar o Compressor

Crie um arquivo `.txt` com algum conteúdo.

Exemplo:

```txt
ola mundo teste huffman
```

Execute:

```bash
./compressor.exe
```

O programa irá solicitar o nome do arquivo:

```txt
=== Compressor de Huffman ===
Digite o nome do arquivo a comprimir: texto.txt
```

Arquivos gerados:

- `teste.txt.huf`
- `teste.txt.arv`

## Executar o Descompressor

Execute:

```bash
./descompressor.exe
```

O programa irá solicitar o nome do arquivo comprimido:

```txt
=== Descompressor de Huffman ===
Digite o nome do arquivo a descomprimir: texto.txt.huf
```

O programa irá gerar o arquivo recuperado com o conteúdo original.

## Observações

- Os arquivos `.huf` e `.arv` devem permanecer na mesma pasta.
- Utilizar Git Bash ou terminal compatível.
- O arquivo `.txt` não pode estar vazio.
