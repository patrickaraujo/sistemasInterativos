package model;

public class ProcessoFactory implements AbstractFactory<Processo> {
	private static int id = 1;

	@Override
	public Processo criaProcesso(int surto) {
		final String nome = "P";
		int mb = 8;

		Processo novoProcesso = new Processo(id, nome + id, mb,
				Processo.Prioridade.MEDIA, Processo.Estado.NOVO, surto);
		
		id++;
		return novoProcesso;
	}

}
