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
import gurobi.GRBLinExpr;

public class Otimizacao {

	private SubArea[] subAreas;
	private Equipamento[] equipamentos;
	private Periodo[] periodos;
	private Rota[] rotas;
	private Integer[]  piers;
	private ArrayList<Integer> produtos;
	
	//Variaveis
	GRBVar[][][] f, e, ip;
	GRBVar[][] ir;
	
	public Otimizacao(SubArea[] subAreas, Equipamento[] equipamentos, Periodo[] periodos, Rota[] rotas,
			ArrayList<Integer> produtos,	Integer[] piers) {
		
		this.subAreas = subAreas;
		this.equipamentos = equipamentos;
		this.periodos = periodos;
		this.rotas = rotas;
		this.produtos = produtos;
		this.piers = piers;
				
		try {
			GRBEnv env = new GRBEnv("otimizacao.log");
			GRBModel model = new GRBModel(env);
			
			criaVariaveis(model);
			criaConstraints(model);
		} catch (GRBException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
	}
	
	private void criaVariaveis(GRBModel model) throws GRBException{
						
		GRBVar[][][] f = new GRBVar[subAreas.length][periodos.length][produtos.size()];//Variavel 1 na tese
		GRBVar[][][] e = new GRBVar[subAreas.length][periodos.length][produtos.size()];//Variavel 6 na tese
		GRBVar[][] ir = new GRBVar[periodos.length][produtos.size()];//Variavel 7 na tese
		GRBVar[][][] ip = new GRBVar[periodos.length][piers.length][produtos.size()];//Variavel 8 na tese
		
		for(int i=0; i< subAreas.length; i++) {
			for(int j=0;j<periodos.length;j++)
			for(Integer w : produtos){
				 {//SubArea i no periodo i tem o produto w
					if(j==0) {
						if((w==subAreas[i].getProdutoEstocado())) {//verifica se a variavel criada é do produto estocado inicialmente na subarea
							f[i][j][w] = model.addVar(1.0, 1.0, 0.0, GRB.BINARY, "f"+Integer.toString(i)+Integer.toString(j)+Integer.toString(w));
							e[i][j][w]=model.addVar(0.0, subAreas[i].getCapacidade(), subAreas[i].getQuantidadeEstocada(), GRB.INTEGER, "e"+Integer.toString(i)+Integer.toString(j)+Integer.toString(w));
						}
						else {
							f[i][j][w]=model.addVar(0.0, 0.0, 0.0, GRB.BINARY, "f"+Integer.toString(i)+Integer.toString(j)+Integer.toString(w));
							e[i][j][w]=model.addVar(0.0, 0.0, 0.0, GRB.INTEGER, "e"+Integer.toString(i)+Integer.toString(j)+Integer.toString(w));
						}
					}
					else {
						f[i][j][w]=model.addVar(0.0, 1.0, 0.0, GRB.BINARY, "f"+Integer.toString(i)+Integer.toString(j)+Integer.toString(w));
						e[i][j][w]=model.addVar(0.0, subAreas[i].getCapacidade(), 0.0, GRB.INTEGER, "e"+Integer.toString(i)+Integer.toString(j)+Integer.toString(w));						
					}
				}
			}
		}
		
		for(int i=0;i<periodos.length;i++) {
			for(int j : produtos) {
				int quant = 0;//soma da quantidade do produtos no periodo e nos anteriores
				for(int w=i;w>=0;w--) {//percorre os periodos anteriodes
					if(periodos[w].getChegadaProdutos().containsKey(j)) {
						quant += periodos[w].getChegadaProdutos().get(j);
					}
				}
				ir[i][j] = model.addVar(0.0, quant, 0.0, GRB.INTEGER, "ir"+Integer.toString(i)+Integer.toBinaryString(j));
			}
		}
		
		
		for(int i=0;i<periodos.length; i++) {
			Navio[] navios = periodos[i].getNavios();
			for(int j=0;j<piers.length;j++) {
				boolean temNavio = false;
				int navio =-1;
				for(int k=0;i<navios.length;k++) {//verifica se algum navio vai atracar no pier j no periodo i
					if(navios[k].getPier()==j) {
						temNavio = true;
						navio = k;
						break;
					}
				}
				if(temNavio) {//se algum navio vai atracar
					for(int w : produtos) {
						
						if(navios[navio].getProdutosQuantidade().containsKey(w)) {//verifica se o navio  precisa do produto W, se sim o maximo que ele pode não receber é o total. senão é 0
							ip[i][j][w] = model.addVar(0.0, navios[j].getProdutosQuantidade().get(w), 0.0, GRB.INTEGER,"ip"+Integer.toString(i)+Integer.toString(j)+Integer.toString(w));
						}
						else {
							ip[i][j][w]=model.addVar(0.0, 0.0, 0.0, GRB.INTEGER,"ip"+Integer.toString(i)+Integer.toString(j)+Integer.toString(w));
						}
					}
				}
				else {
					for(int w : produtos) {
						ip[i][j][w] = model.addVar(0.0, 0.0, 0.0, GRB.INTEGER, "ip"+Integer.toString(i)+Integer.toString(j)+Integer.toString(w));
					}
				}
				
			}
		}
			
	}
	private void criaConstraints(GRBModel model) throws GRBException {
		
		GRBVar[][][] f = this.f;
		
		
		GRBLinExpr expr;
		
		//Constraint 4.7
		for(int i=0; i<f.length; i++) {
			for(int j=0; j<f[i].length;j++) {
				expr = new GRBLinExpr();
				for(int w=0; w<f[i][j].length;j++) {
					expr.addTerm(1.0, f[i][j][w]);
				}
				model.addConstr(expr, GRB.EQUAL, 1.0, "4.7-"+Integer.toString(i)+Integer.toString(j));
			}
		}
	}
}
