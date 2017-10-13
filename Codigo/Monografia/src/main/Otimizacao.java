package main;

import java.util.ArrayList;

import dominio.Equipamento;
import dominio.Navio;
import dominio.Periodo;
import dominio.Rota;
import dominio.SubArea;
import gurobi.GRB;
import gurobi.GRBEnv;
import gurobi.GRBException;
import gurobi.GRBModel;
import gurobi.GRBVar;

public class Otimizacao {

	private SubArea[] subAreas;
	private Equipamento[] equipamentos;
	private Periodo[] periodos;
	private Rota[] rotas;
	private Integer[] produtos;
	private ArrayList<Navio>[] navios;
	
	public Otimizacao(SubArea[] subAreas, Equipamento[] equipamentos, Periodo[] periodos, Rota[] rotas, Integer[] produtos, ArrayList<Navio>[] navios) {
		
		this.subAreas = subAreas;
		this.equipamentos = equipamentos;
		this.periodos = periodos;
		this.rotas = rotas;
		this.produtos = produtos;
		this.navios = navios;
		
		try {
			GRBEnv env = new GRBEnv("otimizacao.log");
			GRBModel model = new GRBModel(env);
			
			criaVariaveis(model);
		} catch (GRBException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
	}
	
	private void criaVariaveis(GRBModel model) throws GRBException{
						
		ArrayList<GRBVar> f = new ArrayList<GRBVar>();//Variavel 1 na tese
		ArrayList<GRBVar> e = new ArrayList<GRBVar>();//Variavel 6 na tese
		ArrayList<GRBVar> ip = new ArrayList<GRBVar>();//Variavel 8 na tese
		
		for(int i=0; i< subAreas.length; i++) {
			for(int j=0;j<produtos.length;j++) {
				for(int w=0;w<periodos.length;j++) {
					if(w==0) {
						if((produtos[j]==subAreas[i].getProdutoEstocado())) {//verifica se a variavel criada é do produto estocado inicialmente na subarea
							f.add(model.addVar(1.0, 1.0, 1.0, GRB.BINARY, "f"+Integer.toString(i)+Integer.toString(produtos[j])+Integer.toString(w)));
							e.add(model.addVar(0.0, subAreas[i].getCapacidade(), subAreas[i].getQuantidadeEstocada(), GRB.INTEGER, "e"+Integer.toString(i)+Integer.toString(produtos[j])+Integer.toString(w)));
						}
						else {
							f.add(model.addVar(0.0, 0.0, 0.0, GRB.BINARY, "f"+Integer.toString(i)+Integer.toString(produtos[j])+Integer.toString(w)));
							e.add(model.addVar(0.0, 0.0, 0.0, GRB.INTEGER, "e"+Integer.toString(i)+Integer.toString(produtos[j])+Integer.toString(w)));
						}
					}
					f.add(model.addVar(0.0, 1.0, 1.0, GRB.BINARY, "f"+Integer.toString(i)+Integer.toString(produtos[j])+Integer.toString(w)));
					e.add(model.addVar(0.0, subAreas[i].getCapacidade(), 0.0, GRB.INTEGER, "e"+Integer.toString(i)+Integer.toString(produtos[j])+Integer.toString(w)));						
				}
			}
		}
		
		
		for(int i=0;i<periodos.length; i++) {
			for(int j=0;j<navios[i].size();j++) {
				for(int w=0; w<produtos.length;w++) {
					if(navios[i].get(j).getProdutosQuantidade().containsKey(w)) {
						ip.add(model.addVar(0.0, navios[i].get(j).getProdutosQuantidade().get(w), 0.0, GRB.INTEGER,"ip"+Integer.toString(j)+Integer.toString(w)+Integer.toString(i)));
					}
					else {
						ip.add(model.addVar(0.0, 0.0, 0.0, GRB.INTEGER,"ip"+Integer.toString(j)+Integer.toString(w)+Integer.toString(i)));
					}
				}
			}
		}
			
	}
}
