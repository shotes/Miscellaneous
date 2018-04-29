//Daniel Ehrlich


import java.util.*;

public class Test3{
	public static void main(String args[]){
		//Creating different Shop types
		Shop<DrPepper> dpShop = new Shop<DrPepper>();
		Shop<Soda> sodaShop = new Shop<Soda>();
		Shop<Drink> drinkShop = new Shop<Drink>();
		
		//creating different Collection objects
		List<Drink> drinkList = new ArrayList<Drink>();
		List<Soda> sodaVector = new Vector<Soda>();
		Set<DrPepper> pepperSet = new HashSet<DrPepper>();
		for(int x = 0; x < 10; x++){
			drinkList.add(new Drink());
			sodaVector.add(new Soda());
			pepperSet.add(new DrPepper());
		}
		
		//Testing different methods- commented ones will give 
		//compile error b/c they disobey class hierarchy
		//Selling-----------------------------------
		drinkShop.sell(drinkList);
		drinkShop.sell(sodaVector);
		drinkShop.sell(pepperSet);
		drinkShop.sell(new Drink());
		drinkShop.sell(new Soda());
		drinkShop.sell(new DrPepper());
		//sodaShop.sell(drinkList);
		sodaShop.sell(sodaVector);
		sodaShop.sell(pepperSet);
		//sodaShop.sell(new Drink());
		sodaShop.sell(new Soda());
		sodaShop.sell(new DrPepper());
		//dpShop.sell(drinkList);
		//dpShop.sell(sodaVector);
		dpShop.sell(pepperSet);
		//dpShop.sell(new Drink());
		//dpShop.sell(new Soda());
		dpShop.sell(new DrPepper());
		System.out.println("Drink:" + drinkShop);
		System.out.println("Soda:" + sodaShop);
		System.out.println("DrPepper:" + dpShop);
		System.out.println("drinkList:" + drinkList);
		System.out.println("sodaVector:" + sodaVector);
		System.out.println("pepperSet:" + pepperSet);
		
		//Buying-----------------------------------------------
		List<Drink> drinkB = new ArrayList<Drink>();
		List<Soda> sodaB= new Vector<Soda>();
		Set<DrPepper> pepperB = new HashSet<DrPepper>();
		
		//buy drinks
		drinkB.add(drinkShop.buy());
		drinkB.add(sodaShop.buy());
		drinkB.add(dpShop.buy());
		drinkShop.buy(2,drinkB);
		sodaShop.buySuper(2,drinkB);
		dpShop.buySuper(2,drinkB);
		
		//buy sodas
		//sodaB.add((Soda)drinkShop.buy());
		sodaB.add(sodaShop.buy());
		sodaB.add(dpShop.buy());
		drinkShop.buy(2,sodaB);
		sodaShop.buy(2,sodaB);
		dpShop.buySuper(2,sodaB);
		
		//buy dr peppers
		//pepperB.add((DrPepper)drinkShop.buy());
		//pepperB.add((DrPepper)sodaShop.buy());
		pepperB.add(dpShop.buy());
		drinkShop.buy(2,pepperB);
		sodaShop.buy(2,pepperB);
		dpShop.buy(2,pepperB);
		System.out.println("Drink:" + drinkShop);
		System.out.println("Soda:" + sodaShop);
		System.out.println("DrPepper" + dpShop);
		System.out.println("drinkB:" + drinkB);
		System.out.println("sodaB:" + sodaB);
		System.out.println("pepperB:" + pepperB);
	}
	
	static class Drink{
		public String toString(){
			return "Drink";
		}
	}
	static class Soda extends Drink{
		public String toString(){
			return "Soda";
		}
	}
	static class DrPepper extends Soda{
		public String toString(){
			return "DrPepper";
		}
	}
}
