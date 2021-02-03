# Protocolo de comunicação

A comunicação entre clientes e servidor deve obedecer a um protocolo de texto simples
(plain text single line protocol), ou seja um protocolo baseado em mensagens de texto
legíveis por um humano, por oposição a protocolos binários (que são mais eficientes do
ponto de vista da comunicação apesar de ilegíveis).

Cada mensagem tem um comprimento máximo de 512 bytes; esta é iniciada por um
comando com 4 caracteres seguida de um ou mais parâmetros separados por espaços
em branco. Uma mensagem pode dar origem (ou não) a uma resposta do servidor.

## Comandos de utilizador (todos os utilizadores):
NICK nome - atribui ou muda o nome/nickname com um máximo de 9 caracteres.
MSSG mensagem - envia uma mensagem para o canal ativo (a mensagem re-enviada
pelo servidor deve ser composta pela mensagem original precedida da identificação de
quem a enviou).
Comandos de utilizador registado (pode ser operador ou não):
PASS password - autenticação de um utilizador registado.
JOIN canal - muda o canal ativo.
LIST - lista os canais existentes.
WHOS - lista e mostra informação dos utilizadores conectados no canal.

## Comandos de Operador:
KICK nome - expulsa o utilizador (remove da lista dos utilizadores registados).
REGS nome password - regista o utilizador (inclui na lista dos utilizadores registados).
OPER nome - promove o utilizador (registado) à categoria de operador.
QUIT - desiste de ser operador.
Mensagens do servidor
RPLY código (código de resposta) / [listas] - respostas a comandos.
MSSG "origem nome/categoria:> mensagem" - outras mensagens.

# Sistema base a implementar [8 valores] 
O servidor deve aceitar ligações de vários clientes através do porto TCP 5555, e o
sistema implementar os comandos NICK e MSSG.

## NICK "nome" 
Atribui ou muda o nome/nickname com um máximo de 9 caracteres (apenas letras ASCII 
e algarismos). Numa nova ligação a definição do nome é obrigatoriamente o primeiro
pedido que o cliente tem de fazer. Só após a definição do nome é que o cliente recebe e
envia mensagens.
Respostas do servidor:
RPLY 001 - Nome atribuído com sucesso
RPLY 002 - Erro: Falta introdução do nome.
RPLY 003 - Erro: Nome pedido não válido (caso não tenha apenas letras e algarismos e 9
caracteres no máximo)
RPLY 004 - Erro: nome já em uso (num outro utilizador registado ou em uso por um
utilizador não registado, e o comando não tem qualquer efeito
Mensagens do servidor enviadas a todos os clientes ativos no mesmo canal do cliente
que faz o pedido,
MSSG "server :> novo utilizador <nome>"
se for um utilizador novo, ou
MSSG "server :> <nome_antigo> mudou o seu nome para <nome>"
se já tinha nome atribuído.

## MSSG "mensagem"

Envia uma mensagem para o canal ativo (a mensagem será re-enviada pelo servidor,
composta pela mensagem original precedida da identificação de quem a enviou) O total
do comando e da mensagem original não pode exceder os 512 bytes.
Respostas do servidor:
RPLY 101 - mensagem enviada com sucesso.
RPLY 102 - Erro. Não há texto na mensagem.
RPLY 103 - Erro. Mensagem demasiado longa.
Mensagem do servidor enviada a todos os clientes:
MSSG "<nome_da_origem/categoria> :> mensagem_original"

# Registo de clientes/autenticação [4 valores]
O servidor deve possuir um sistema para guardar em disco os dados dos clientes
registados (nome e password, e categoria). O sistema deve implementar os comandos
PASS, e JOIN.

## PASS "password"
Autenticação de um utilizador registado (só após o nome estar definido) password com
um máximo de 9 caracteres
Respostas do servidor:
RPLY 201 - Autenticação com sucesso.
RPLY 202 - Erro. Nome não está definido.
RPLY 203 - Erro. Password incorreta.

## JOIN "canal"

Muda o canal ativo para "canal"
Respostas do servidor:
RPLY 301 - Mudança de canal com sucesso
RPLY 302 – Erro. canal não existente
RPLY 303 - Erro. Não pode mudar para o canal (utilizador não autenticado)
Mensagem do servidor enviada a todos os clientes ativos no <canal>:
MSSG "server :> <nome> entrou neste canal"
Mensagem do servidor enviada a todos os clientes ativos no <canal_anterior>:
MSSG "server :> <nome> deixou este canal"

# Gestão de utilizadores registados [3 valores]
O servidor deve fazer a gestão de utilizadores registados (criar e/ou remover) por um
operador. O sistema deve implementar os comandos REGS, e KICK, (note que o sistema
deve ser inicializado com um operador por default, por exemplo “admin” com password
“admin”).

# KICK "nome"
Operador expulsa o utilizador <nome> (remove da lista dos utilizadores registados)
Respostas do servidor:
RPLY 601 – Utilizador expulso.
RPLY 602 – Erro. Ação não autorizada, utilizador cliente não é um operador.
Mensagem do servidor enviada a todos os clientes:
MSSG "server :> <nome> foi expulso”

# REGS "nome" "password"
Regista o utilizador (inclui na lista dos utilizadores registados)
Respostas do servidor:
RPLY 701 – Utilizador foi registado com sucesso.
RPLY 702 – Erro. Ação não autorizada, utilizador cliente não é um operador.
Mensagem do servidor enviada a todos os clientes:
MSSG "server :> <nome> foi registado”
# Gestão de utilizadores operadores [3 valores]
O servidor deve conceder poder de gestão de operadores (promover e/ou desistir) aos
operadores existentes. O sistema deve implementar os comandos OPER, e QUIT.

#  TO DO LIST

## Interface

- [ ] visualizar o fluxo de mensagens por parte do cliente 
- [ ] dar os comandos ao cliente (menu)

## client
- [ ] NICK "nome" 
- [ ] PASS password - autenticação de um utilizador registado.
- [ ] JOIN canal - muda o canal ativo.
- [ ] LIST - lista os canais existentes.
- [ ] WHOS - lista e mostra informação dos utilizadores conectados no canal.

## server
- [ ] MSSG "server" 