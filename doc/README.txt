ricardo.dparizotto@gmail.com


A organização dos diretórios deve ser a seguinte:

-udp_router/
	-bin/
		-router.sh
	-config/
		-enlaces.config
		-roteador.config

os arquivos .sh são executáveis
os arquivos com extensão .config são arquivos de configuração


> O arquivo enlaces.config armazena a topologia da rede da seguinte maneira:
	id1(unsigned int) id2(unsigned int) distância(unsigned int)		

> O arquivo roteador.config armazena as informações da rede (ip e portas para comunicação) da seguinte maneira:
	id(unsigned int) port(unsigned int) ip(string(15))  	


O executável router.sh deve ser instanciado com o ID do roteador em questão.

	./router.sh 1


O shell está sempre esperando uma mensagem do usuário. A mensagem deve ser acompanhada pelo ID do roteador que ela vai ser enviada
	
	<router_id> <mensagem>

O roteador tentará enviar no máximo três vezes a mensagem para o destino. Cada reenvio é acompanhado por uma mensagem no shell.

	> Resend package <pkg_id>.


Se em três tentativas a mensagem não chegar ao destino, a seguinte mensagem será mostrada:
	
	> Package <pkg_id> can't reach destiny.
	
Quando um pacote é recebido, as informações de quem enviou são mostradas na tela:
	
	> Received packet from <ip_address> : <port>

Caso a mensagem chegar no destino, também é mostrado a mensagem.

	DATA: <mensagem>

Quando um roteador recebe uma mensagem de confirmação:

	> Acknowledgment <pkg_id>
	




