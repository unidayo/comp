
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.util.*;

public class Main {
	static InputStream is;
	static PrintWriter out;
	static String INPUT = "";

	static boolean debug = false;

	static int n;
	static int[][] a;

	static long S;

	static void solve()
	{
		n = ni();
		a = new int[n][];
		for(int i = 0;i < n;i++)a[i] = na(2);
		S = System.currentTimeMillis();

		prepare();

		while(System.currentTimeMillis() - S < 1600){
			greedy();
//			tr(ps.size());
			remove();
//			tr(ps.size());
		}

		//		tr(System.currentTimeMillis()-T);
		//
//		sa();

		if(debug){
			long best = cost();
			out.println(ps.size());
			out.println(best);
		}else {
			print();
		}
	}

	static void sa()
	{
		Random gen = new Random();
		long base = cost();
		int ct = 0;
		tr("base", base);
		inner:
		while(System.currentTimeMillis() - S < 60000){
			int type = 0;//gen.nextInt(2);

			if(type == 1){
				int i = gen.nextInt(n);
				int j = gen.nextInt(n);
				for(int k = 0;k < ps.size();k++){
					if(ps.get(k)[0] == xs[i] && ps.get(k)[1] == ys[j])continue inner;
				}
				ct++;
				ps.add(new int[]{xs[i], ys[j]});
				long cost = cost();
				if(cost < base) {
					tr(type, base, cost, ps.size());
					base = cost;
				}else{
					ps.remove(ps.size()-1);
				}
			}else if(ps.size()-n > 0){
				int i = n+gen.nextInt(ps.size()-n);
				int[] p = ps.remove(i);
				ct++;

				long cost = cost();
				if(cost <= base) {
					tr(type, base, cost, ps.size());
					base = cost;
				}else{
					ps.add(i, p);
				}
			}
		}
		tr("ct", ct, base);
	}

	static List<int[]> ps;

	static long[] mcs;

	static int[][] costs;
	static long[][] gains;
	static L[][] arggains;

	static void calcmc(int i)
	{
		int[] v = ps.get(i);
		mcs[i] = v[0] + v[1];
		for(int j = 0;j < ps.size();j++){
			int[] u = ps.get(j);
			if(u[0] <= v[0] && u[1] <= v[1] && !(u[0] == v[0] && u[1] == v[1])){
				mcs[i] = Math.min(mcs[i], -u[0] - u[1] + v[0] + v[1]);
			}
		}
	}

	static void qgain(int i, PriorityQueue<Datum> q)
	{
		int x = ps.get(i)[0];
		int y = ps.get(i)[1];
		int xi = Arrays.binarySearch(xs, x);
		int yi = Arrays.binarySearch(ys, y);
		for(int j = xi;j >= 0 && -xs[j] + x < mcs[i];j--){
			for(int k = yi;k >= 0;k--){
				if(j == xi && k == yi)continue;
				if(-xs[j] - ys[k] + x + y < mcs[i]){
					if(costs[j][k] < gains[j][k] && costs[j][k] != 0){
						q.add(new Datum(xs[j], ys[k], gains[j][k] - costs[j][k]));
					}
				}else{
					break;
				}
			}
		}
	}

	static void addgain(int i)
	{
		int x = ps.get(i)[0];
		int y = ps.get(i)[1];
		int xi = Arrays.binarySearch(xs, x);
		int yi = Arrays.binarySearch(ys, y);
		for(int j = xi;j >= 0 && -xs[j] + x < mcs[i];j--){
			for(int k = yi;k >= 0;k--){
				if(j == xi && k == yi)continue;
				if(-xs[j] - ys[k] + x + y < mcs[i]){
					gains[j][k] += mcs[i] - (-xs[j] - ys[k] + x + y);
					if(arggains[j][k] == null){
						arggains[j][k] = new L(1);
					}
					arggains[j][k].add(i);
				}else{
					break;
				}
			}
		}
	}

	static void precalc()
	{
		mcs = new long[5*n];
		Arrays.fill(mcs, Long.MAX_VALUE);
		for(int i = 0;i < ps.size();i++){
			calcmc(i);
		}
		costs = new int[n][n];
		gains = new long[n][n];
		arggains = new L[n][n];

		for(int i = 0;i < ps.size();i++){
			addgain(i);
		}

		for(int i = 0;i < n;i++) {
			Arrays.fill(costs[i], Integer.MAX_VALUE);
		}

		costs[0][0] = 0;
		for(int i = 0;i < n;i++){
			int xi = Arrays.binarySearch(xs, a[i][0]);
			int yi = Arrays.binarySearch(ys, a[i][1]);
			costs[xi][yi] = 0;
		}
		for(int i = 0;i < n;i++){
			for(int j = 0;j < n;j++){
				if(i-1 >= 0){
					costs[i][j] = Math.min(costs[i][j], costs[i-1][j] + xs[i] - xs[i-1]);
				}
				if(j-1 >= 0){
					costs[i][j] = Math.min(costs[i][j], costs[i][j-1] + ys[j] - ys[j-1]);
				}
			}
		}
	}

	public static class L {
		public int[] a;
		public int p = 0;

		public L(int n) { a = new int[n]; }
		public L add(int n)
		{
			if(p >= a.length)a = Arrays.copyOf(a, a.length*3/2+1);
			a[p++] = n;
			return this;
		}
		public int size() { return p; }
		public L clear() { p = 0; return this; }
		public int[] toArray() { return Arrays.copyOf(a, p); }
		@Override
		public String toString() {return Arrays.toString(toArray());}

		public void remove(int x)
		{
			for(int i = 0;i < p;i++){
				if(a[i] == x){
					a[i] = a[--p];
					return;
				}
			}
		}
	}


	static class Datum
	{
		int x, y;
		long cha;

		public Datum(int x, int y, long cha) {
			this.x = x;
			this.y = y;
			this.cha = cha;
		}
	}

	static void remove()
	{
		outer:
		for(int i = n;i < ps.size();i++){
			int[] p = ps.get(i);
			int x = p[0];
			int y = p[1];
			long plus = mcs[i];

			for(int j = 0;j < ps.size();j++){
				if(i == j)continue;
				int[] q = ps.get(j);
				int xx = q[0];
				int yy = q[1];
				if(x <= xx && y <= yy && mcs[j] == xx + yy - x - y){
					long nmc = xx + yy;
					for(int k = 0;k < ps.size();k++){
						if(j == k || k == i)continue;
						int[] r = ps.get(k);
						int xxx = r[0];
						int yyy = r[1];
						if(xxx <= xx && yyy <= yy){
							nmc = Math.min(nmc, xx + yy - (xxx + yyy));
						}
					}
					plus -= nmc - mcs[j];
					if(plus < 0)continue outer;
				}
			}
			if(plus >= 0){
//				long bef = cost();
				ps.remove(i);
//				long af = cost();
//				tr(bef, af, plus);
				for(int j = 0;j < ps.size();j++){
					calcmc(j);
				}
			}
		}
	}

	static void prepare()
	{
		xs = new int[n];
		ys = new int[n];
		int p = 0;
		for(int i = 0;i < n;i++){
			xs[p] = a[i][0];
			ys[p++] = a[i][1];
		}
		Arrays.sort(xs);
		Arrays.sort(ys);

		ps = new ArrayList<>();
		for(int i = 0;i < n;i++){
			ps.add(a[i]);
		}
	}

	static void greedy()
	{
		/*
		   6x...
		   5.x..
		   2..x.
		   0x..x
		    0234
		 */

		precalc();
//		tr(costs);
//		tr(gains);
//		tr(arggains);

		PriorityQueue<Datum> q = new PriorityQueue<>((x, y) -> -Long.compare(x.cha, y.cha));
		for(int i = 0;i < n;i++){
			for(int j = 0;j < n;j++){
				if(costs[i][j] < gains[i][j] && costs[i][j] != 0){
					q.add(new Datum(xs[i], ys[j], gains[i][j] - costs[i][j]));
				}
			}
		}
//		tr(q.size());

		while(ps.size() < 5*n && !q.isEmpty() && System.currentTimeMillis() - S < 1600){
			Datum cur = q.poll();
			// lazy update
			int cxi = Arrays.binarySearch(xs, cur.x);
			int cyi = Arrays.binarySearch(ys, cur.y);
//			tr("hey", cur.x, cur.y, cur.cha);
			{
				if(costs[cxi][cyi] == 0)continue;
				if(costs[cxi][cyi] >= gains[cxi][cyi])continue;
				if(gains[cxi][cyi] - costs[cxi][cyi] != cur.cha){
					cur.cha = gains[cxi][cyi] - costs[cxi][cyi];
					q.add(cur);
					continue;
				}
			}
//			tr(cur.cha);
			ps.add(new int[]{cur.x, cur.y});
			int[] tar = arggains[cxi][cyi].toArray();

			// calc mc
			calcmc(ps.size()-1);
			addgain(ps.size()-1);

//			tr(costs);
//			tr(gains);
//			tr(arggains);
//			tr(mcs);
//			tr(tar);

			for(int i : tar){
				int x = ps.get(i)[0];
				int y = ps.get(i)[1];
				int xi = Arrays.binarySearch(xs, x);
				int yi = Arrays.binarySearch(ys, y);
				for(int j = xi;j >= 0 && -xs[j] + x < mcs[i];j--){
					for(int k = yi;k >= 0;k--){
						if(j == xi && k == yi)continue;
						if(-xs[j] - ys[k] + x + y < mcs[i]){
							gains[j][k] -= mcs[i] - (-xs[j] - ys[k] + x + y);
							arggains[j][k].remove(i);
						}else{
							break;
						}
					}
				}

				calcmc(i);
				addgain(i);
//				qgain(i, q);
			}

			// update cost
			costs[cxi][cyi] = 0;

			Queue<int[]> que = new ArrayDeque<>();
			que.add(new int[]{cxi, cyi});
			while(!que.isEmpty()){
				int[] curp = que.poll();
				int cx = curp[0];
				int cy = curp[1];
				if(costs[cx][cy] < gains[cx][cy] && costs[cx][cy] != 0){
					q.add(new Datum(xs[cx], ys[cy], gains[cx][cy] - costs[cx][cy]));
				}
				if(cx+1 < n && costs[cx][cy] + xs[cx+1] - xs[cx] < costs[cx+1][cy]){
					costs[cx+1][cy] = costs[cx][cy] + xs[cx+1] - xs[cx];
					que.add(new int[]{cx+1, cy});
				}
				if(cy+1 < n && costs[cx][cy] + ys[cy+1] - ys[cy] < costs[cx][cy+1]){
					costs[cx][cy+1] = costs[cx][cy] + ys[cy+1] - ys[cy];
					que.add(new int[]{cx, cy+1});
				}
			}

//			outer:
//			for(int i = cxi;i < n;i++){
//				for(int j = cyi;j < n;j++){
//					long o = costs[i][j];
//					if(i-1 >= 0){
//						costs[i][j] = Math.min(costs[i][j], costs[i-1][j] + xs[i] - xs[i-1]);
//					}
//					if(j-1 >= 0){
//						costs[i][j] = Math.min(costs[i][j], costs[i][j-1] + ys[j] - ys[j-1]);
//					}
//					if(o >= gains[i][j] && costs[i][j] < gains[i][j] && costs[i][j] != 0){
//						q.add(new Datum(xs[i], ys[j], gains[i][j] - costs[i][j]));
//					}
//					if(o == costs[i][j]){
//						if(i > cxi && j == cyi)break outer;
//						break;
//					}
//				}
//			}

			qgain(ps.size()-1, q);

			//			tr(mcs);
//			tr(costs);
//			tr(gains);
//			tr(arggains);

//			if(debug){
//				out.println(ps.size() + " " + q.size() + " " + cost());
//			}
		}
//		tr(ps.size());
	}

	static void print() {
		List<int[]> use = new ArrayList<>();
		for (int i = 0; i < ps.size(); i++) {
			use.add(ps.get(i));
		}
		use.sort((x, y) -> x[0] != y[0] ? x[0] - y[0] : x[1] - y[1]);

		out.println(use.size());
		for(int i = 0;i < use.size();i++){
			int x = use.get(i)[0];
			int y = use.get(i)[1];
			long minc = x + y;
			int[] larg = new int[]{0, 0};
			for(int j = 0;j < i;j++){
				if(use.get(j)[0] <= x && use.get(j)[1] <= y){
					long v = -use.get(j)[0] - use.get(j)[1] + x + y;
					if(v < minc){
						minc = v;
						larg = use.get(j);
					}
				}
			}
			out.println(larg[0] + " " + larg[1] + " " + x + " " + y);
		}
	}

	static long cost()
	{
		List<int[]> use = new ArrayList<>();
		for(int i = 0;i < ps.size();i++){
			use.add(ps.get(i));
		}
		use.sort((x, y) -> x[0] != y[0] ? x[0] - y[0] : x[1] - y[1]);

		long ans = 0;
		for(int i = 0;i < use.size();i++){
			int x = use.get(i)[0];
			int y = use.get(i)[1];
			long minc = x + y;
			for(int j = 0;j < i;j++){
				if(use.get(j)[0] <= x && use.get(j)[1] <= y){
					minc = Math.min(minc, -use.get(j)[0] - use.get(j)[1] + x + y);
				}
			}
			ans += minc;
		}
		return ans;
	}

	static int[] xs, ys;

	public static void trnz(int... o)
	{
		for(int i = 0;i < o.length;i++)if(o[i] != 0)System.out.print(i+":"+o[i]+" ");
		System.out.println();
	}

	public static void main(String[] args) throws Exception
	{
		long S = System.currentTimeMillis();
		is = INPUT.isEmpty() ? System.in : new ByteArrayInputStream(INPUT.getBytes());
		out = new PrintWriter(System.out);
		
		solve();
		out.flush();
		long G = System.currentTimeMillis();
		tr(G-S+"ms");
	}
	
	private static boolean eof()
	{
		if(lenbuf == -1)return true;
		int lptr = ptrbuf;
		while(lptr < lenbuf)if(!isSpaceChar(inbuf[lptr++]))return false;
		
		try {
			is.mark(1000);
			while(true){
				int b = is.read();
				if(b == -1){
					is.reset();
					return true;
				}else if(!isSpaceChar(b)){
					is.reset();
					return false;
				}
			}
		} catch (IOException e) {
			return true;
		}
	}
	
	private static byte[] inbuf = new byte[1024];
	static int lenbuf = 0, ptrbuf = 0;
	
	private static int readByte()
	{
		if(lenbuf == -1)throw new InputMismatchException();
		if(ptrbuf >= lenbuf){
			ptrbuf = 0;
			try { lenbuf = is.read(inbuf); } catch (IOException e) { throw new InputMismatchException(); }
			if(lenbuf <= 0)return -1;
		}
		return inbuf[ptrbuf++];
	}
	
	private static boolean isSpaceChar(int c) { return !(c >= 33 && c <= 126); }
//	private static boolean isSpaceChar(int c) { return !(c >= 32 && c <= 126); }
	private static int skip() { int b; while((b = readByte()) != -1 && isSpaceChar(b)); return b; }
	
	private static double nd() { return Double.parseDouble(ns()); }
	private static char nc() { return (char)skip(); }
	
	private static String ns()
	{
		int b = skip();
		StringBuilder sb = new StringBuilder();
		while(!(isSpaceChar(b))){
			sb.appendCodePoint(b);
			b = readByte();
		}
		return sb.toString();
	}
	
	private static char[] ns(int n)
	{
		char[] buf = new char[n];
		int b = skip(), p = 0;
		while(p < n && !(isSpaceChar(b))){
			buf[p++] = (char)b;
			b = readByte();
		}
		return n == p ? buf : Arrays.copyOf(buf, p);
	}
	
	private static char[][] nm(int n, int m)
	{
		char[][] map = new char[n][];
		for(int i = 0;i < n;i++)map[i] = ns(m);
		return map;
	}
	
	private static int[] na(int n)
	{
		int[] a = new int[n];
		for(int i = 0;i < n;i++)a[i] = ni();
		return a;
	}
	
	private static int ni()
	{
		int num = 0, b;
		boolean minus = false;
		while((b = readByte()) != -1 && !((b >= '0' && b <= '9') || b == '-'));
		if(b == '-'){
			minus = true;
			b = readByte();
		}
		
		while(true){
			if(b >= '0' && b <= '9'){
				num = num * 10 + (b - '0');
			}else{
				return minus ? -num : num;
			}
			b = readByte();
		}
	}
	
	private static long nl()
	{
		long num = 0;
		int b;
		boolean minus = false;
		while((b = readByte()) != -1 && !((b >= '0' && b <= '9') || b == '-'));
		if(b == '-'){
			minus = true;
			b = readByte();
		}
		
		while(true){
			if(b >= '0' && b <= '9'){
				num = num * 10 + (b - '0');
			}else{
				return minus ? -num : num;
			}
			b = readByte();
		}
	}
	
	private static void tr(Object... o) { if(INPUT.length() != 0)System.out.println(Arrays.deepToString(o)); }
}
