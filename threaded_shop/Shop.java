//Daniel Ehrlich


import java.util.LinkedList;
import java.util.List;
import java.util.Collection;
import java.util.Iterator;
public class Shop<T>{
	List<T> stock;
	public Shop(){
		stock = new java.util.LinkedList<T>();
	}
	public T buy(){
		return stock.remove(0);
	}
	<S extends T> void buy(int n, Collection<S> items){
		for(T e : stock.subList(0, n)){
			items.add((S)e);
		}
		for(int i=0; i<n; ++i)
			stock.remove(0);
	}
	void buySuper(int n, Collection<? super T> items){
		for(T e : stock.subList(0, n)){
			items.add(e);
		}
		for(int i=0; i<n; ++i)
			stock.remove(0);
	}
	void sell(T item){
		stock.add(item);
	}
	void sell(Collection<? extends T> items){
		for(T e : items){
			stock.add(e);
		}
	}
	public String toString(){
		return "Shop: " + stock;
	}
}
