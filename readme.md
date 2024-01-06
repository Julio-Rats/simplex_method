# Implementação do Método Simplex

Este repositório contém uma implementação em C do Método Simplex para otimização de funções lineares. O Método Simplex é um algoritmo utilizado para resolver problemas de pesquisa operacional (PO) na qual pode ser usado para maximizar ou minimizar uma função objetivo sujeita a um conjunto de restrições lineares de suas varáveis.

Como usar o programa
Para utilizar o programa, basta fornecer um arquivo texto contendo a função objetivo e as restrições. O arquivo deve estar formatado da seguinte maneira:
~~~txt
max 5x1 + 3x2

2x1 + x2 <= 10
x1 + 3x2 <= 12
~~~
A declaração da função objetivo pode-se usar **max**, **maximize**, **min** ou **minimize**.

### Compilar e executar o programa:

A compilação pode ser feita através do **Makefile** usando programa **make**, ou manualmente acessando
a pasta com terminal e usando o compilador, exemplo:
~~~sh
gcc -c *.o
gcc *.o -o simplex.exe
~~~

Esses comando irá gerar o executável simplex.exe, no qual deve ser passado por parâmetro o arquivo
com a função objetivo e suas restrições
~~~sh
simplex.exe restricoes.txt
~~~

O programa executará mostrando os passos executáveis em notação matricial, dando no final a solução
se está existir, vale ressaltar que o método ainda NÂO trata casos degenerados, não mostrando todas
as soluções em caso de múltiplas soluções.
