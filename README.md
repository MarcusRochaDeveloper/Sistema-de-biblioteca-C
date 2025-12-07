# 📚 Sistema de Gerenciamento de Biblioteca em C

> **Nota:** Este projeto foi desenvolvido como um **Trabalho Acadêmico** para a disciplina de - LINGUAGEM E TÉCNICAS DE PROGRAMAÇÃO]. O objetivo é demonstrar conhecimentos em lógica de programação, manipulação de arquivos e estruturas de dados em C.
 
---

## 📋 Sobre o Projeto

Este é um sistema simples baseado em console (CLI) para o gerenciamento de uma biblioteca pequena a média. O software permite o cadastro de livros e usuários, além do controle de empréstimos e devoluções, garantindo a persistência dos dados através de arquivos binários/texto.

### 🚀 Funcionalidades Principais

* **Gerenciamento de Acervo:** Cadastro e consulta de livros.
* **Gerenciamento de Usuários:** Cadastro de leitores.
* **Empréstimos:** Registro de saída de livros vinculados a um usuário.
* **Devoluções e Multas:** Verificação de atrasos baseada na data atual.
* **Persistência de Dados:** Salvamento automático em arquivo para manter os registros após fechar o programa.

---

## ⚙️ Configurações e Limites Técnicos

Para garantir a estabilidade e o escopo do projeto acadêmico, o sistema opera com os seguintes limites predefinidos (hardcoded):

| Entidade | Capacidade Máxima |
| :--- | :--- |
| **Livros** | 1.000 registros |
| **Usuários** | 1.000 registros |
| **Empréstimos** | 2.000 registros históricos |

*As strings (nomes, títulos, etc.) possuem limites de buffer seguros para evitar overflow.*

---

## ⚠️ Observação Importante sobre Testes de Atraso

O sistema utiliza a **data e hora do Sistema Operacional** para todas as validações (data de cadastro, data de empréstimo e cálculo de dias de atraso).
