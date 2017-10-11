package dominio;

import java.util.ArrayList;
import java.util.HashMap;

public class Periodo {

	//<Produto, Quantidade>
	private HashMap<Integer, Integer> chegadaProdutos;

	private ArrayList<Navio> navios;
	
	public ArrayList<Navio> getNavios() {
		return navios;
	}

	public void setNavios(ArrayList<Navio> navios) {
		this.navios = navios;
	}

	public HashMap<Integer, Integer> getChegadaProdutos() {
		return chegadaProdutos;
	}

	public void setChegadaProdutos(HashMap<Integer, Integer> chegadaProdutos) {
		this.chegadaProdutos = chegadaProdutos;
	}
	
	
}
