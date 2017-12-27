package dominio;

import java.util.HashMap;

public class Periodo {
	
	public static final int TEMPO_PERIODO = 12;
	
	private HashMap<Integer, Integer> chegadaProdutos;

	private Navio[] navios;
	
	public Navio[] getNavios() {
		return navios;
	}

	public void setNavios(Navio[] navio) {
		this.navios = navio;
	}

	public HashMap<Integer, Integer> getChegadaProdutos() {
		return chegadaProdutos;
	}

	public void setChegadaProdutos(HashMap<Integer, Integer> chegadaProdutos) {
		this.chegadaProdutos = chegadaProdutos;
	}
	
	
}
