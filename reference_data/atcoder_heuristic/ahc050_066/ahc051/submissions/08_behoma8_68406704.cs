/* ==========================================================================
	ahc051
========================================================================== */
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Intrinsics.X86;
using System.Runtime.Intrinsics;
using static System.Console;

#if TEST
using System.Windows.Forms;
#endif

//#nullable enable
#nullable disable

namespace Alg {

	//@@ ******************************************************************************************
	//@@ static class Prm
	//@@ ******************************************************************************************
	public static class Prm {
		public static bool isReadPrm = false;
		public static bool isPrint = false;
		public static bool isInfo = false;
		public static bool isError = false;

		public static bool isDebug = false;
		public static bool isTest = false;
		public static bool isSim = false;
		public static bool isOptuna = false;
		public static bool isFis = false;
		public static bool isClip = false;
		public static bool isLoad = false;
		public static bool isTim = false;

		public static int P = 1;
		public static int Q = 1;
		public static int R = 1;

		public static double prmValue0 = 0.15;
		public static double prmValue1 = 1600.0;
		public static double prmValue2 = 0;
		public static int prmParam0 = 32;
		public static int prmParam1 = 0;
		public static int prmParam2 = 0;
		public static bool prmBool0 = false;

		public static bool prmNextTraPlaceSorter    = false;
		public static bool prmNextTraSwapProc       = false;
		public static bool prmNextTraMergeBelt      = false;
		public static bool prmNextTraSwitchBelt     = false;
		public static bool prmNextTraAcceptSelf     = false;

		public static double prmDistXZoomRatio      = 1.0;
		public static bool prmGenRootSprout         = false;
//		public static bool prmIntersectsQuadTree    = false;
//		public static bool prmIntersectsRTree       = false;
//		public static bool prmIntersectsUniformGrid = true;

		public static int prmUniformGridSize        = 10000/16;
		public static int prmNextTraSwitchMaxTry    = 2;
		public static int prmNextTraSwitchMaxNear   = 8;
		public static int prmUseSorterNum           = 1000;
		public static double prmNextTraSwitchBeltP  = 1.0; // 0.8ぐらいが最適？
		public static double prmNextTraTGain        = 0.0; // M で調整？

		public static double prmSaMiEnergyGain      = 3.0e-7;
		public static double prmSaMiAlpha           = 0.9965;

		public static (bool,string) Parse( ReadOnlyDictionary<string,List<string>> opts ) {
			try {
				foreach ( var kv in opts ) {
					bool isarg0 = ( 0 < kv.Value.Count );
					bool isarg1 = ( 1 < kv.Value.Count );
					bool isarg2 = ( 2 < kv.Value.Count );
					string arg0 = ( isarg0 ) ? kv.Value[0] : String.Empty;
					string arg1 = ( isarg1 ) ? kv.Value[1] : String.Empty;
					string arg2 = ( isarg2 ) ? kv.Value[2] : String.Empty;
					switch ( kv.Key ) {

					case "prmValue0": if ( isarg0 ) prmValue0 = Double.Parse( arg0 ); break;
					case "prmValue1": if ( isarg0 ) prmValue1 = Double.Parse( arg0 ); break;
					case "prmValue2": if ( isarg0 ) prmValue2 = Double.Parse( arg0 ); break;
					case "prmParam0": if ( isarg0 ) prmParam0 = Int32.Parse( arg0 ); break;
					case "prmParam1": if ( isarg0 ) prmParam1 = Int32.Parse( arg0 ); break;
					case "prmParam2": if ( isarg0 ) prmParam2 = Int32.Parse( arg0 ); break;

					case "prmSaMiEnergyGain": if ( isarg0 ) prmSaMiEnergyGain = Double.Parse( arg0 ); break;
					case "prmSaMiAlpha": if ( isarg0 ) prmSaMiAlpha = Double.Parse( arg0 ); break;

					case "p":
						isReadPrm = true;
						if ( isarg0 ) P = Int32.Parse( arg0 );
						if ( isarg1 ) Q = Int32.Parse( arg1 );
						if ( isarg2 ) R = Int32.Parse( arg2 );
						break;
					case "noPrint":
						isPrint = false;
						break;
					case "noinfo":
						isInfo = false;
						break;
					case "noerror":
						isError = false;
						break;
					case "debug":
						isDebug = true;
						break;
					case "print":
						isPrint = true;
						break;
					case "info":
						isInfo = true;
						break;
					case "error":
						isError = true;
						break;
					case "load":
						isLoad = true;
						break;
					case "d":
						isDebug = true;
						break;
					case "t":
						isTest = true;
						break;
					case "nosim":
						isSim = false;
						break;
					case "sim":
						isSim = true;
						break;
					case "optuna":
						isOptuna = true;
						break;
					case "tim":
						isTim = true;
						break;
					case "fis":
						isFis = true;
						break;
					case "clip":
						isClip = true;
						break;

					default:
						return ( false, String.Format( "unknown arg = {0}", kv.Key ) );
					}
				}
				return ( true, "" );
			} catch ( Exception e ) {
				Console.Write( e );
				return ( false, e.Message );
			}
		}
		public static void Disp() {
			Type t = typeof(Prm);
			Console.WriteLine( t );
			var infos = t.GetFields( BindingFlags.Static | BindingFlags.Public );
			foreach ( var i in infos) {
				Console.WriteLine( "    {0,-12} = {1}", i.Name, i.GetValue( t ) );
			}
		}
		public static void LoadParamOptuna( int N, int K, int H, int T, int D ) {
			if ( isOptuna ) return;
		}
		public static double Interpolate01( double alpha, double x0, double x1 ) {
			return x0 + alpha * (x1-x0);
		}
		public static double InterpolateXY( double x, double x0, double y0, double x1, double y1  ) {
			return ( y1 - y0 ) / ( x1 - x0 ) * ( x - x0 ) + y0;
		}
		public static double EstScore( double N, double M, double K ) {
			var NN = 1.0 * N * N;
			var MM = 1.0 * M * M;
			var KK = 1.0 * K * K;
			var c0 = 37921610.0634523;
			var c1 = 42134915.0168384;
			var c2 = -646363.061356009;
			var c3 = -6249745.29604029;
			var c4 = -738658.187;
			var c5 = 407.021;
			var c6 = 52868.60558;
			var sum = c0 + c1 * N + c2 * M + c3 * K + c4 * NN + c5 * MM + c6 * KK;
			return sum;
		}
	}

	//@@ **********************************************************************
	//@@ class std
	//@@ **********************************************************************
	public static class std {

		// MinMax ----------------------------------------------------------------
		public static T MinMax<T>( T min, T x, T max ) where T : INumber<T> {
			if ( max < x ) {
				return max;
			} else if ( x < min ) {
				return min;
			} else {
				return x;
			}
		}
		public static void ChMaxKey<K,V>( ref K key, ref V val, K k, V v ) where K : INumber<K> {
			if ( key<k ) {
				(key,val) = (k,v);
			}
		}
		public static void ChMinKey<K,V>( ref K key, ref V val, K k, V v ) where K : INumber<K> {
			if ( k<key ) {
				(key,val) = (k,v);
			}
		}

		// Range ----------------------------------------------------------------
		public static IEnumerable<int> Range( int n ) {
			return Enumerable.Range( 0, n );
		}
		public static IEnumerable<int> Range( int s, int e ) {
			return Enumerable.Range( s, e - s );
		}

		// Arr ----------------------------------------------------------------
		public static T[] Arr1<T>( int dim0 ) {
			var dats = new T[dim0];
			return dats;
		}
		public static T[] Arr1<T>( int dim0, T v ) {
			var dats = new T[dim0];
			for ( int i=0; i<dim0; i++ ) dats[i] = v;
			return dats;
		}
		public static T[] Arr1<T>( int dim0, Func<int,T> f ) {
			var dats = new T[dim0];
			for ( int i=0; i<dim0; i++ ) dats[i] = f(i);
			return dats;
		}

		public static T[][] Arr2<T>( int dim0 ) {
			var dats = new T[dim0][];
			return dats;
		}
		public static T[][] Arr2<T>( int dim0, int dim1 ) {
			var dats = new T[dim0][];
			for ( int i=0; i<dim0; i++ ) dats[i] = Arr1<T>( dim1 );
			return dats;
		}
		public static T[][] Arr2<T>( int dim0, int dim1, T v ) {
			var dats = new T[dim0][];
			for ( int i=0; i<dim0; i++ ) dats[i] = Arr1( dim1, v );
			return dats;
		}
		public static T[][] Arr2<T>( int dim0, int dim1, Func<int,int,T> f ) {
			var dats = new T[dim0][];
			for ( int i=0; i<dim0; i++ ) dats[i] = new T[dim1];
			for ( int i=0; i<dim0; i++ ) for ( int j=0; j<dim1; j++ ) dats[i][j] = f(i,j);
			return dats;
		}

		public static T[][][] Arr3<T>( int dim0, int dim1, int dim2 ) {
			var dats = new T[dim0][][];
			for ( int i=0; i<dim0; i++ ) dats[i] = Arr2<T>( dim1, dim2 );
			return dats;
		}
		public static T[][][] Arr3<T>( int dim0, int dim1, int dim2, T v ) {
			var dats = new T[dim0][][];
			for ( int i=0; i<dim0; i++ ) dats[i] = Arr2<T>( dim1, dim2, v );
			return dats;
		}
		public static T[][][] Arr3<T>( int dim0, int dim1, int dim2, Func<int,int,int,T> f ) {
			var dats = new T[dim0][][];
			for ( int i=0; i<dim0; i++ ) dats[i] = Arr2<T>( dim1, dim2 );
			for ( int i=0; i<dim0; i++ ) for ( int j=0; j<dim1; j++ ) for ( int k=0; k<dim2; k++ ) dats[i][j][k] = f(i,j,k);
			return dats;
		}

		// List ----------------------------------------------------------------
		public static List<T> List1<T>() {
			return new List<T>();
		}
		public static List<T> List1<T>( int dim0, T v=default(T) ) {
			var ls = new List<T>( dim0 );
			if ( v!=null ) {
				for(int i=0; i<dim0; i++ ) {
					ls.Add( v );
				}
			} else {
				std.Error( $"Alg.List1<T> -> v = null" );
			}
			return ls;
		}
		public static List<T> List1<T>( int dim0, Func<int,T> f ) {
			var ls = new List<T>( dim0 );
			for(int i=0; i<dim0; i++ ) {
				ls.Add( f( i ) );
			}
			return ls;
		}

		public static List<List<T>> List2<T>( int dim0 ) {
			var ls = new List<List<T>>( dim0 );
			for ( int i=0; i<dim0; i++ ) {
				ls.Add( new List<T>() );
			}
			return ls;
		}
		public static List<List<T>> List2<T>( int dim0, int dim1, T v=default(T) ) {
			var ls = new List<List<T>>( dim0 );
			for ( int i=0; i<dim0; i++ ) ls.Add( List1<T>( dim1, v ) );
			return ls;
		}
		public static List<List<T>> List2<T>( int dim0, int dim1, Func<int,int,T> f ) {
			var ls = new List<List<T>>( dim0 );
			for ( int i=0; i<dim0; i++ ) ls.Add( List1<T>( dim1 ) );
			for ( int i=0; i<dim0; i++ ) for ( int j=0; j<dim1; j++ ) ls[i][j] = f(i,j);
			return ls;
		}

		// Read ----------------------------------------------------------------
		public static string Read() => Console.ReadLine()??"";
		public static string ReadS() => Console.ReadLine()??"";
		public static double ReadD() => Double.Parse( Console.ReadLine()??"" );
		public static int    ReadI() => Int32.Parse( Console.ReadLine()??"" );
		public static long   ReadL() => Int64.Parse( Console.ReadLine()??"" );
		public static ulong  ReadU() => UInt64.Parse( Console.ReadLine()??"" );
		public static string[] ReadSX() => (Console.ReadLine()??"").Split().ToArray();
		public static double[] ReadDX() => (Console.ReadLine()??"").Split().Select( i => Double.Parse( i ) ).ToArray();
		public static float [] ReadFX() => (Console.ReadLine()??"").Split().Select( i => Single.Parse( i ) ).ToArray();
		public static int   [] ReadIX() => (Console.ReadLine()??"").Split().Select( i => Int32.Parse( i ) ).ToArray();
		public static long  [] ReadLX() => (Console.ReadLine()??"").Split().Select( i => Int64.Parse( i ) ).ToArray();
		public static ulong [] ReadUX() => (Console.ReadLine()??"").Split().Select( i => UInt64.Parse( i ) ).ToArray();
		public static string[] ReadSY( int n ) => Enumerable.Range( 0, n ).Select( i => Console.ReadLine()??"" ).ToArray();
		public static double[] ReadDY( int n ) => Enumerable.Range( 0, n ).Select( i => Double.Parse( Console.ReadLine()??"" ) ).ToArray();
		public static int   [] ReadIY( int n ) => Enumerable.Range( 0, n ).Select( i => Int32.Parse( Console.ReadLine()??"" ) ).ToArray();
		public static long  [] ReadLY( int n ) => Enumerable.Range( 0, n ).Select( i => Int64.Parse( Console.ReadLine()??"" ) ).ToArray();
		public static ulong [] ReadUY( int n ) => Enumerable.Range( 0, n ).Select( i => UInt64.Parse( Console.ReadLine()??"" ) ).ToArray();
		public static string[][] ReadSXY( int n ) => Enumerable.Range( 0, n ).Select( i => ReadSX() ).ToArray();
		public static double[][] ReadDXY( int n ) => Enumerable.Range( 0, n ).Select( i => ReadDX() ).ToArray();
		public static float [][] ReadFXY( int n ) => Enumerable.Range( 0, n ).Select( i => ReadFX() ).ToArray();
		public static int   [][] ReadIXY( int n ) => Enumerable.Range( 0, n ).Select( i => ReadIX() ).ToArray();
		public static long  [][] ReadLXY( int n ) => Enumerable.Range( 0, n ).Select( i => ReadLX() ).ToArray();
		public static ulong [][] Readuxy( int n ) => Enumerable.Range( 0, n ).Select( i => ReadUX() ).ToArray();
		public static string[] ReadSX( string s ) => s.Split().ToArray();
		public static double[] ReadDX( string s ) => s.Split().Select( i => Double.Parse( i ) ).ToArray();
		public static int   [] ReadIX( string s ) => s.Split().Select( i => Int32.Parse( i ) ).ToArray();
		public static long  [] ReadLX( string s ) => s.Split().Select( i => Int64.Parse( i ) ).ToArray();
		public static ulong [] ReadUX( string s ) => s.Split().Select( i => UInt64.Parse( i ) ).ToArray();
		public static string[][] ReadSXY( IEnumerable<string> s ) => s.Select( i => ReadSX( i ) ).ToArray();
		public static double[][] ReadDXY( IEnumerable<string> s ) => s.Select( i => ReadDX( i ) ).ToArray();
		public static int   [][] ReadIXY( IEnumerable<string> s ) => s.Select( i => ReadIX( i ) ).ToArray();
		public static long  [][] ReadLXY( IEnumerable<string> s ) => s.Select( i => ReadLX( i ) ).ToArray();
		public static ulong [][] ReadUXY( IEnumerable<string> s ) => s.Select( i => ReadUX( i ) ).ToArray();

		public static void Deconstruct<T>( this T[] v, out T _0 ) { _0 = v[0]; }
		public static void Deconstruct<T>( this T[] v, out T _0, out T _1 ) { _0 = v[0]; _1 = v[1]; }
		public static void Deconstruct<T>( this T[] v, out T _0, out T _1, out T _2 ) {_0 = v[0];_1 = v[1];_2 = v[2];}
		public static void Deconstruct<T>( this T[] v, out T _0, out T _1, out T _2, out T _3 ) {_0 = v[0];_1 = v[1];_2 = v[2];_3 = v[3];}
		public static void Deconstruct<T>( this T[] v, out T _0, out T _1, out T _2, out T _3, out T _4 ) {_0 = v[0];_1 = v[1];_2 = v[2];_3 = v[3];_4 = v[4];}

		// Print ----------------------------------------------------------------
		public static void sep( string msg="->" ) { Console.WriteLine( $"#{msg} @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" ); }
		public static void beg( string msg="->" ) { Console.WriteLine( $"#Beg {msg} @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" ); }
		public static void end( string msg="->" ) { Console.WriteLine( $"#End {msg} @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" ); }
		public static void sep<T>( T v ) { Console.WriteLine( $"#{v} @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" ); }
		public static void beg<T>( T v ) { Console.WriteLine( $"Beg#{v} @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" ); }
		public static void end<T>( T v ) { Console.WriteLine( $"End#{v} @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" ); }
		public static void sep<T,U>( T t, U u ) { Console.WriteLine( $"#{t} {u} @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" ); }
		public static void sep<T,U,V>( T t, U u, V v ) { Console.WriteLine( $"#{t} {u} {v} @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" ); }

		public static bool isPrint = true;
		public static void Print() { if ( isPrint ) Console.WriteLine(); }
		public static void Print0<T>( T v ) { if ( isPrint ) Console.Write( $"{v}" ); }
		public static void Print<T>( T v ) { if ( isPrint ) Console.WriteLine( $"#{v}" ); }
		public static void Print<T>( T[] v ) { if ( isPrint ) Console.WriteLine( "#" + String.Join( " ", (v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ) ); }
		public static void Print<T>( T[][] v ) { if ( isPrint ) foreach ( var i in v ) Print( i ); }
		public static void Print<T>( T[][][] v ) { if ( isPrint ) foreach ( var i in v ) Print( i ); }
		public static void Print<T>( List<T> v ) { if ( isPrint ) Console.WriteLine( "#" + String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }
		public static void Print<T>( List<List<T>> v ) { if ( isPrint ) foreach ( var i in v ) Print( i ); }
		public static void Print<T>( T v0, T v1 ) { if ( isPrint ) Console.WriteLine( $"#{v0},{v1}" ); }
		public static void Print<T>( T v0, T v1, T v2 ) { if ( isPrint ) Console.WriteLine( $"#{v0},{v1},{v2}" ); }
		public static void Print<T>( T v0, T v1, T v2, T v3 ) { if ( isPrint ) Console.WriteLine( $"#{v0},{v1},{v2},{v3}" ); }
		public static void PrintLn<T>( T[] v ) { if ( isPrint ) foreach ( var i in v ) Console.WriteLine( $"#{i}" ); }
		public static void PrintLn<T>( List<T> v ) { if ( isPrint ) foreach ( var i in v ) Console.WriteLine( $"#{i}" ); }
		public static void Prints<T>( params T[] v ) { if ( isPrint ) Print( v ); }
		public static void PrintIte<T>( IEnumerable<T> v ) { if ( isPrint ) Console.WriteLine( "#" + String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }

		public static void Print<T>( string msg, T v ) { if ( isPrint ) Console.WriteLine( $"#{msg} -> {v}" ); }
		public static void Print<T>( string msg, T[] v ) { if ( isPrint ) Console.WriteLine( $"#{msg} -> " + String.Join( " ", (v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ) ); }
		public static void Print<T>( string msg, T[][] v ) { if ( isPrint ) foreach ( var i in v ) Print( msg, i ); }
		public static void Print<T>( string msg, T[][][] v ) { if ( isPrint ) foreach ( var i in v ) Print( msg, i ); }
		public static void Print<T>( string msg, List<T> v ) { if ( isPrint ) Console.WriteLine( $"#{msg} -> " + String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }
		public static void Print<T>( string msg, List<List<T>> v ) { if ( isPrint ) foreach ( var i in v ) Print( msg, i ); }
		public static void Print<T>( string msg, T v0, T v1 ) { if ( isPrint ) Console.WriteLine( $"#{msg} -> {v0},{v1}" ); }
		public static void Print<T>( string msg, T v0, T v1, T v2 ) { if ( isPrint ) Console.WriteLine( $"#{msg} -> {v0},{v1},{v2}" ); }
		public static void Print<T>( string msg, T v0, T v1, T v2, T v3 ) { if ( isPrint ) Console.WriteLine( $"#{msg} -> {v0},{v1},{v2},{v3}" ); }
		public static void PrintLn<T>( string msg, T[] v ) { if ( isPrint ) foreach ( var i in v ) Console.WriteLine( $"#{msg} -> {i}" ); }
		public static void PrintLn<T>( string msg, List<T> v ) { if ( isPrint ) foreach ( var i in v ) Console.WriteLine( $"#{msg} -> {i}" ); }
		public static void Prints<T>( string msg, params T[] v ) { if ( isPrint ) Print( msg, v ); }
		public static void PrintIte<T>( string msg, IEnumerable<T> v ) { if ( isPrint ) Console.WriteLine( $"#{msg} -> " + String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }

		// Info ----------------------------------------------------------------
		public static bool isInfo = true;
		public static void Info<T>( T v ) { if ( isInfo ) Console.WriteLine( $"#{v}" ); }
		public static void Info<T>( T[] v ) { if ( isInfo ) Console.WriteLine( $"#" + String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }
		public static void Info<T>( T[][] v ) { if ( isInfo ) foreach ( var i in v ) Info( i ); }
		public static void Info<T>( T[][][] v ) { if ( isInfo ) foreach ( var i in v ) Info( i ); }
		public static void Info<T>( List<T> v ) { if ( isInfo ) Console.WriteLine( $"#" + String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }
		public static void Info<T>( T v0, T v1 ) { if ( isInfo ) Console.WriteLine( $"#{v0},{v1}" ); }
		public static void Info<T>( T v0, T v1, T v2 ) { if ( isInfo ) Console.WriteLine( $"#{v0},{v1},{v2}" ); }
		public static void Info<T>( T v0, T v1, T v2, T v3 ) { if ( isInfo ) Console.WriteLine( $"#{v0},{v1},{v2},{v3}" ); }
		public static void InfoLn<T>( T[] v ) { if ( isInfo ) foreach ( var i in v ) Console.WriteLine( $"#{i}" ); }
		public static void InfoLn<T>( List<T> v ) { if ( isInfo ) foreach ( var i in v ) Console.WriteLine( $"#{i}" ); }
		public static void Infos<T>( params T[] v ) { if ( isInfo ) Info( v ); }
		public static void InfoIte<T>( IEnumerable<T> v ) { if ( isInfo ) Console.WriteLine( $"#" + String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }

		// Error ----------------------------------------------------------------
		public static bool isError = true;
		public static void Error<T>( T v ) { if ( isError ) Console.Error.WriteLine( $"#{v}" ); }
		public static void Error<T>( T[] v ) { if ( isError ) Console.Error.WriteLine( "#" + String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }
		public static void Error<T>( T[][] v ) { if ( isError ) foreach ( var i in v ) Error( i ); }
		public static void Error<T>( T[][][] v ) { if ( isError ) foreach ( var i in v ) Error( i ); }
		public static void Error<T>( List<T> v ) { if ( isError ) Console.Error.WriteLine( "#" + String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }
		public static void Error<T>( T v0, T v1 ) { if ( isError ) Console.Error.WriteLine( $"#{v0},{v1}" ); }
		public static void Error<T>( T v0, T v1, T v2 ) { if ( isError ) Console.Error.WriteLine( $"#{v0},{v1},{v2}" ); }
		public static void Error<T>( T v0, T v1, T v2, T v3 ) { if ( isError ) Console.Error.WriteLine( $"#{v0},{v1},{v2},{v3}" ); }
		public static void ErrorLn<T>( T[] v ) { if ( isError ) foreach ( var i in v ) Console.Error.WriteLine( $"#{i}" ); }
		public static void ErrorLn<T>( List<T> v ) { if ( isError ) foreach ( var i in v ) Console.Error.WriteLine( $"#{i}" ); }
		public static void Errors<T>( params T[] v ) { if ( isError ) Error( v ); }
		public static void ErrorIte<T>( IEnumerable<T> v ) { if ( isError ) Console.Error.WriteLine( "#" + String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }

		// Write ----------------------------------------------------------------
		public static void WriteLn() { Console.WriteLine(); }
		public static void WriteLn<T>( T v ) { Console.WriteLine( $"{v}" ); }
		public static void Write<T>( T v ) { Console.Write( $"{v}" ); }
		public static void Write<T>( T[] v ) { Console.Write( String.Join( " ", (v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ) ); }
		public static void Write<T>( T[][] v ) { foreach ( var i in v ) Write( i ); }
		public static void Write<T>( T[][][] v ) { foreach ( var i in v ) Write( i ); }
		public static void Write<T>( List<T> v ) { Console.Write( String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }
		public static void Write<T>( T v0, T v1 ) { Console.Write( $"{v0},{v1}" ); }
		public static void Write<T>( T v0, T v1, T v2 ) { Console.Write( $"{v0},{v1},{v2}" ); }
		public static void Write<T>( T v0, T v1, T v2, T v3 ) { Console.Write( $"{v0},{v1},{v2},{v3}" ); }
		public static void WriteLn<T>( T[] v ) { foreach ( var i in v ) Console.Write( $"{i}" ); Console.WriteLine(); }
		public static void WriteLn<T>( List<T> v ) { foreach ( var i in v ) Console.Write( $"{i}" ); Console.WriteLine(); }
		public static void Writes<T>( params T[] v ) { Print( v ); }
		public static void WriteIte<T>( IEnumerable<T> v ) { Console.Write( String.Join( " ", v.Select( i => (i==null)?"":i.ToString() ).ToArray() ) ); }

		// Tuple ----------------------------------------------------------------
		public static void Deconstruct( this (int,int) e, out int _0 ) { _0 = e.Item1; }
		public static void Deconstruct( this (int,int,int) e, out int _0 ) { _0 = e.Item1; }
		public static void Deconstruct( this (int,int,int) e, out int _0, out int _1 ) { _0 = e.Item1; _1 = e.Item2; }

		// Throw ----------------------------------------------------------------
		public static void Throw( string msg="" ) => throw new Exception( msg );
		public static void Throw( bool b, string msg="" ) { if ( b ) throw new Exception( msg ); }
		public static int throwCnt = 0;
		public static void ThrowInc() => throwCnt++;
		public static void ThrowDec() => throwCnt--;
		public static void ThrowCntEq( int cnt, string msg="" ) { if ( throwCnt==cnt ) throw new Exception( msg ); }
		public static void ThrowCntNot( int cnt, string msg="" ) { if ( throwCnt!=cnt ) throw new Exception( msg ); }


		// Tool ----------------------------------------------------------------
		public static int CalcBitLen( int n ) {
			var bit = 0;
			var v = 1;
			while ( v < n ) {
				v <<= 1;
				bit++;
			}
			return bit;
		}
		public static int CalcBitLenBaseN( int n, int b ) {
			var bit = 0;
			var v = 1;
			while ( v < n ) {
				v *= b;
				bit++;
			}
			return bit;
		}
	}

	//@@ **********************************************************************
	//@@ class StdEx
	//@@ **********************************************************************
	public static class StdEx {

		// Str ----------------------------------------------------------------
		public static string Str<T>( this T[] src ) {
			return String.Join(" ", src.Select(i=>i!.ToString()) );
		}
		public static string Str<T>( this List<T> src ) {
			return String.Join(" ", src.Select(i=>i!.ToString()) );
		}
		public static string Str<T>( this Span<T> src ) {
			return String.Join(" ", src.ToArray().Select(i=>i!.ToString()) );
		}
		public static string Str<T>( this T[][] src, string sep=" | " ) {
			var ls = new List<string>();
			foreach ( var s in src ) {
				ls.Add( String.Join(" ", s.Select(i=>i!.ToString()) ) );
			}
			return String.Join(sep, ls.Select(i=>i!.ToString()) );
		}
		public static string Str<T>( this List<List<T>> src, string sep=" | " ) {
			var ls = new List<string>();
			foreach ( var s in src ) {
				ls.Add( String.Join(" ", s.Select(i=>i!.ToString()) ) );
			}
			return String.Join(sep, ls.Select(i=>i!.ToString()) );
		}
		public static string Str<T>( this List<T>[] src, string sep=" | " ) {
			var ls = new List<string>();
			foreach ( var s in src ) {
				ls.Add( String.Join(" ", s.Select(i=>i!.ToString()) ) );
			}
			return String.Join(sep, ls.Select(i=>i!.ToString()) );
		}

		// Shuffle ----------------------------------------------------------------
		public static void Shuffle0<T>( this T[] ar ) => Shuffle.All( ar );
		public static void Shuffle0<T>( this List<T> ls ) => Shuffle.All( ls );
		public static void Shuffle0<T>( this Span<T> sp ) => Shuffle.All( sp );

		public static void Shuffle0<T>( this T[] ar, int beg, int len ) => Shuffle.Range( ar, beg, len );
		public static void Shuffle0<T>( this List<T> ls, int beg, int len ) => Shuffle.Range( ls, beg, len );
		public static void Shuffle0<T>( this Span<T> sp, int beg, int len ) => Shuffle.Range( sp, beg, len );

		// Ch ----------------------------------------------------------------
		public static void ChMax<T>( this ref T x, T y ) where T : struct, INumber<T> {
			if ( x < y ) x = y;
		}
		public static void ChMin<T>( this ref T x, T y ) where T : struct, INumber<T> {
			if ( y < x ) x = y;
		}
		public static void ChMinMax<T>( this ref T x, T y0, T y1 ) where T : struct, INumber<T> {
			if ( y1 < x ) {
				x = y1;
			} else if ( x < y0 ) {
				x = y0;
			}
		}
		public static void ChMaxKey<K,V>( this ref (K,V) x, K k, V v ) where K : struct, INumber<K> where V : struct {
			if ( x.Item1 < k ) {
				x = (k,v);
			}
		}
		public static void ChMinKey<K,V>( this ref (K,V) x, K k, V v ) where K : struct, INumber<K> where V : struct {
			if ( k < x.Item1 ) {
				x = (k,v);
			}
		}
		public static void ChMaxKeyCls<K,V>( this ref (K,V) x, K k, V v ) where K : struct, INumber<K> where V : class {
			if ( x.Item1 < k ) {
				x = (k,v);
			}
		}
		public static void ChMinKeyCls<K,V>( this ref (K,V) x, K k, V v ) where K : struct, INumber<K> where V : class {
			if ( k < x.Item1 ) {
				x = (k,v);
			}
		}

		public static void ChMaxKey<K,V>( this ref K key, ref V val, K k, V v ) where K : struct, INumber<K> {
			if ( key<k ) {
				(key,val) = (k,v);
			}
		}
		public static void ChMinKey<K,V>( this ref K key, ref V val, K k, V v ) where K : struct, INumber<K> {
			if ( k<key ) {
				(key,val) = (k,v);
			}
		}


		// MinMax ----------------------------------------------------------------
		public static T Inf<T>( this T x ) where T : IMinMaxValue<T> => T.MaxValue;
		public static T Ninf<T>( this T x ) where T : IMinMaxValue<T> => T.MinValue;
		public static T Eps<T>( this T x ) where T : IFloatingPointIeee754<T> => T.Epsilon;
		public static T Nan<T>( this T x ) where T : IFloatingPointIeee754<T> => T.NaN;
		public static T NZero<T>( this T x ) where T : IFloatingPointIeee754<T> => T.NegativeZero;

		public static T Max<T>( this T x, T y ) where T : INumber<T> {
			return ( x < y ) ? y : x;
		}
		public static T Min<T>( this T x, T y ) where T : INumber<T> {
			return ( y < x ) ? y : x;
		}
		public static T MinMax<T>( this T x, T y0, T y1 ) where T : INumber<T> {
			return ( x < y0 ) ? y0 : ( y1 < x ) ? y1 : x;
		}

		public static (int,T) MinIdx<T>( this Span<T> sp ) where T : INumber<T> {
			var idx = 0;
			var val = sp[0];
			for ( int i=1; i<sp.Length; i++ ) {
				if ( sp[i] < val ) {
					val = sp[i];
					idx = i;
				}
			}
			return (idx,val);
		}
		public static (int,T) MaxIdx<T>( this Span<T> sp ) where T : INumber<T> {
			var idx = 0;
			var val = sp[0];
			for ( int i=1; i<sp.Length; i++ ) {
				if ( val < sp[i] ) {
					val = sp[i];
					idx = i;
				}
			}
			return (idx,val);
		}
		public static (int,T) MinIdx<T,U>( this Span<T> sp, Func<T,U> fn ) where U : INumber<U> {
			var idx = 0;
			var val = fn( sp[0] );
			for ( int i=1; i<sp.Length; i++ ) {
				var v = fn( sp[i] );
				if ( v < val ) {
					val = v;
					idx = i;
				}
			}
			return (idx,sp[idx]);
		}
		public static (int,T) MaxIdx<T,U>( this Span<T> sp, Func<T,U> fn ) where U : INumber<U> {
			var idx = 0;
			var val = fn( sp[0] );
			for ( int i=1; i<sp.Length; i++ ) {
				var v = fn( sp[i] );
				if ( val < v ) {
					val = v;
					idx = i;
				}
			}
			return (idx,sp[idx]);
		}

		public static (int,T) MinIdx<T>( this T[] arr ) where T : INumber<T> => arr.AsSpan().MinIdx();
		public static (int,T) MaxIdx<T>( this T[] arr ) where T : INumber<T> => arr.AsSpan().MaxIdx();
		public static (int,T) MinIdx<T>( this List<T> list ) where T : INumber<T> => list.AsSpan().MinIdx();
		public static (int,T) MaxIdx<T>( this List<T> list ) where T : INumber<T> => list.AsSpan().MaxIdx();
		public static (int,T) MinIdx<T,U>( this T[] arr, Func<T,U> fn ) where U : INumber<U> => arr.AsSpan().MinIdx( fn );
		public static (int,T) MaxIdx<T,U>( this T[] arr, Func<T,U> fn ) where U : INumber<U> => arr.AsSpan().MaxIdx( fn );
		public static (int,T) MinIdx<T,U>( this List<T> list, Func<T,U> fn ) where U : INumber<U> => list.AsSpan().MinIdx( fn );
		public static (int,T) MaxIdx<T,U>( this List<T> list, Func<T,U> fn ) where U : INumber<U> => list.AsSpan().MaxIdx( fn );

		// Span ----------------------------------------------------------------
		public static Span<T> AsSpan<T>( this T[] arr ) {
			return new Span<T>( arr );
		}
		public static Span<T> AsSpan<T>( this T[] arr, int beg, int len ) {
			return new Span<T>( arr, beg, len );
		}
		public static ReadOnlySpan<T> AsReadOnlySpan<T>( this T[] arr ) {
			return new ReadOnlySpan<T>( arr );
		}
		public static ReadOnlySpan<T> AsReadOnlySpan<T>( this T[] arr, int beg, int len ) {
			return new ReadOnlySpan<T>( arr, beg, len );
		}

		public static Span<T> AsSpan<T>( this List<T> lis ) {
			return CollectionsMarshal.AsSpan<T>( lis );
		}
		public static Span<T> AsSpan<T>( this List<T> lis, int beg, int len ) {
			return lis.AsSpan().Slice( beg, len );
		}
		public static ReadOnlySpan<T> AsReadOnlySpan<T>( this List<T> lis ) {
			return CollectionsMarshal.AsSpan<T>( lis );
		}
		public static ReadOnlySpan<T> AsReadOnlySpan<T>( this List<T> lis, int beg, int len ) {
			return lis.AsReadOnlySpan().Slice( beg, len );
		}

		// SpanSort ----------------------------------------------------------------
		public unsafe static void SpanSort<T>( Span<T> spn, Func<T,int> fn ) {
			Span<int> keys = stackalloc int[spn.Length];
			for ( int i=0; i<spn.Length; i++ ) keys[i] = fn( spn[i] );
			keys.Sort( spn );
		}
		public unsafe static void SpanSort<T>( Span<T> spn, Func<T,double> fn ) {
			Span<double> keys = stackalloc double[spn.Length];
			for ( int i=0; i<spn.Length; i++ ) keys[i] = fn( spn[i] );
			keys.Sort( spn );
		}

		public static void SpanSort<T,U>( Span<T> spn, Func<T,U> fn ) where U : INumber<U> {
			Span<U> keys = new U[spn.Length];
			for ( int i=0; i<spn.Length; i++ ) keys[i] = fn( spn[i] );
			keys.Sort( spn );
		}
		public static void SpanSort<T,U>( T[] arr, Func<T,U> fn ) where U : INumber<U> {
			SpanSort( new Span<T>( arr ), fn );
		}
		public static void SpanSort<T,U>( List<T> lis, Func<T,U> fn ) where U : INumber<U> {
			SpanSort( CollectionsMarshal.AsSpan<T>( lis ), fn );
		}

		public static void Sort<T,U>( this Span<T> spn, Func<T,U> fn ) where U : INumber<U> {
			SpanSort( spn, fn );
		}
		public static void Sort<T,U>( this T[] arr, Func<T,U> fn ) where U : INumber<U> {
			SpanSort( arr, fn );
		}
		public static void Sort<T,U>( this List<T> lis, Func<T,U> fn ) where U : INumber<U> {
			SpanSort( lis, fn );
		}

		// For ----------------------------------------------------------------
		public static void For<T>( this Span<T> spn, Action<T> act ) {
			for ( int i=0; i<spn.Length; i++ ) act( spn[i] );
		}
		public static void For<T>( this Span<T> spn, Action<T,int> act ) {
			for ( int i=0; i<spn.Length; i++ ) act( spn[i], i );
		}
		public static void For<T>( this Span<T> spn, Func<T,T> fn ) {
			for ( int i=0; i<spn.Length; i++ ) spn[i] = fn( spn[i] );
		}
		public static void For<T>( this Span<T> spn, Func<T,int,T> fn ) {
			for ( int i=0; i<spn.Length; i++ ) spn[i] = fn( spn[i], i );
		}
		public static void For<T>( this T[] arr, Action<T> act ) => arr.AsSpan().For( act );
		public static void For<T>( this T[] arr, Action<T,int> act ) => arr.AsSpan().For( act );
		public static void For<T>( this T[] arr, Func<T,T> fn ) => arr.AsSpan().For( fn );
		public static void For<T>( this T[] arr, Func<T,int,T> fn ) => arr.AsSpan().For( fn );
		public static void For<T>( this List<T> lis, Action<T> act ) => lis.AsSpan().For( act );
		public static void For<T>( this List<T> lis, Action<T,int> act ) => lis.AsSpan().For( act );
		public static void For<T>( this List<T> lis, Func<T,T> fn ) => lis.AsSpan().For( fn );
		public static void For<T>( this List<T> lis, Func<T,int,T> fn ) => lis.AsSpan().For( fn );

		// Dup ----------------------------------------------------------------
		public static T[] Dup<T>( this T[] arr ) {
			var a = std.Arr1<T>( arr.Length );
			a.CopyFrom( arr );
			return a;
		}
		public static T[][] Dup<T>( this T[][] arr ) {
			var a = std.Arr2<T>( arr.Length, arr[0].Length );
			a.CopyFrom( arr );
			return a;
		}
		public static T[][][] Dup<T>( this T[][][] arr ) {
			var a = std.Arr3<T>( arr.Length, arr[0].Length, arr[0][0].Length );
			a.CopyFrom( arr );
			return a;
		}
		public static List<T> Dup<T>( this List<T> src ) {
			var ls = new List<T>( src.Count );
			for ( int i=0; i<src.Count; i++ ) {
				ls.Add( src[i] );
			}
			return ls;
		}
		public static List<List<T>> Dup<T>( this List<List<T>> src ) {
			var ls = new List<List<T>>( src.Count );
			for ( int i=0; i<src.Count; i++ ) {
				ls.Add( src[i].ToList() );
			}
			return ls;
		}

		// Fill ----------------------------------------------------------------
		public static void Fill<T>( this T[] arr, T v ) {
			Array.Fill( arr, v );
		}
		public static void Fill<T>( this T[][] arr, T v ) {
			for ( int i=0; i<arr.Length; i++ ) arr[i].Fill( v );
		}
		public static void Fill<T>( this T[][][] arr, T v ) {
			for ( int i=0; i<arr.Length; i++ ) arr[i].Fill( v );
		}
		public static void Fill<T>( this T[] arr, Func<int,T,T> f ) {
			for ( int i=0; i<arr.Length; i++ ) arr[i] = f(i,arr[i]);
		}
		public static void Fill<T>( this T[][] arr, Func<int,int,T,T> f ) {
			for ( int i=0; i<arr.Length; i++ ) for ( int j=0; j<arr[i].Length; j++ ) arr[i][j] = f(i,j,arr[i][j]);
		}
		public static void Fill<T>( this List<T> ls, T v ) {
			ls.AsSpan().Fill( v );
		}
		public static void Fill<T>( this Span<T> sp, T v ) {
			sp.Fill( v );
		}

		// CopyFrom ----------------------------------------------------------------
		public static void CopyFrom<T>( this T[] dst, T[] src, int srcPos, int dstPos, int len ) {
			Array.Copy( src, srcPos, dst, dstPos, len );
		}
		public static void CopyFrom<T>( this T[] dst, T[] src ) {
			var size = Math.Min( dst.Length, src.Length );
			Array.Copy( src, 0, dst, 0, size );
		}
		public static void CopyFrom<T>( this T[][] dst, T[][] src ) {
			var size = Math.Min( dst.Length, src.Length );
			for ( int i=0; i<size; i++ ) dst[i].CopyFrom( src[i] );
		}
		public static void CopyFrom<T>( this T[][][] dst, T[][][] src ) {
			var size = Math.Min( dst.Length, src.Length );
			for ( int i=0; i<size; i++ ) dst[i].CopyFrom( src[i] );
		}
		public static void CopyFrom<T>( this List<T> dst, List<T> src ) {
			src.AsSpan().CopyTo( dst.AsSpan() );
		}
		public static void CopyFrom<T>( this Span<T> dst, Span<T> src ) {
			src.CopyTo( dst );
		}

		// List ---------------------------------------------------------------
		public static void Resize<T>( this List<T> ls, int size ) {
			size = size - ls.Count;
			if ( 0 < size ) {
				for ( int i=0; i<size; i++ ) ls.Add( default );
			}
		}
		public static T Back<T>( this List<T> ls ) {
			return ls[ls.Count-1];
		}
		public static void Trim<T>( this List<T> ls, int n ) {
			if ( n < ls.Count ) {
				ls.RemoveRange( n, ls.Count - n );
			}
		}
		public static void Trim<T>( this List<T> ls, Func<T,bool> pred ) {
			var p = 0;
			for ( int i=0; i<ls.Count; i++ ) {
				if ( ! pred( ls[i] ) ) {
					ls[p] = ls[i];
					p++;
				}
			}
			ls.RemoveRange( p, ls.Count - p );
		}
		public static (List<T>,List<T>) Split2<T>( this List<T> ls, int index ) {
			var b0 = new List<T>( index );
			var b1 = new List<T>( ls.Count - index );
			for ( int i=0; i<index; i++ ) b0.Add( ls[i] );
			for ( int i=index; i<ls.Count; i++ ) b1.Add( ls[i] );
			return (b0,b1);
		}
		public static (List<T>,List<T>,List<T>) Split3<T>( this List<T> ls, int index0, int index1 ) {
			var b0 = new List<T>( index0 );
			var b1 = new List<T>( index1 - index0 );
			var b2 = new List<T>( ls.Count - index1 );
			for ( int i=0; i<index0; i++ ) b0.Add( ls[i] );
			for ( int i=index0; i<index1; i++ ) b1.Add( ls[i] );
			for ( int i=index1; i<ls.Count; i++ ) b2.Add( ls[i] );
			return (b0,b1,b2);
		}
	}

	//@@ ***********************************************************************
	//@@ class CommandOpt2
	//@@ ***********************************************************************
	public static class CommandOpt2Ex {
		public static void Deconstruct( this KeyValuePair<string,List<string>> p, out string _0, out List<string> _1 ) {
			_0 = p.Key;
			_1 = p.Value; 
		}
	}
	public class CommandOpt2 {
		//@@ struct ------------------------------------------------------------
		public struct IdxPos {
			public readonly int idx;
			public readonly int pos;
			public IdxPos( int i, int p ) {
				idx = i;
				pos = p;
			}
		}
		//@@ field ------------------------------------------------------------
		private readonly string[] args_;
		private Dictionary<string,IdxPos> dict_ = new Dictionary<string,IdxPos>();
		private List<int> pointer_ = new List<int>();

		//@@ construtor -------------------------------------------------------
		public CommandOpt2( string[] args, char optCh='-' ) {
			if ( args == null ) {
				args_ = new string[0];
			} else {
				args_ = args;
				Parse( args, optCh );
			}
		}

		//@@ property ---------------------------------------------------------
		public int Len { get { return args_.Length; } }
		public object this[ int n ] { get { return args_[n]; } }

		//@@ private method -------------------------------------------------------
		private bool StartsWithOptStr( char optCh, string str, out string key ) {
			int pos = 0;
			while ( pos < str.Length ) {
				if ( str[pos] != optCh ) {
					break;
				}
				pos++;
			}
			key = str.Substring( pos, str.Length - pos );
			return ( pos != 0 ) && ( pos != str.Length );
		}

		private int GetNextPosByIdx( int idx ) {
			if ( ( idx + 1 ) < pointer_.Count ) {
				return pointer_[idx+1];
			} else {
				return args_.Length;
			}
		}
		private int GetPosByIdx( int idx ) {
			if ( idx < pointer_.Count ) {
				return pointer_[idx];
			} else {
				return args_.Length;
			}
		}

		//@@ public method -------------------------------------------------------
		public void Parse( string[] args, char optCh='-' ) {
			int idx = 0;
			for ( int i=0; i<args_.Length; i++ ) {
				string key;
				if ( StartsWithOptStr( optCh, args_[i], out key ) ) {
					dict_[key] = new IdxPos( idx, i );
					pointer_.Add( i );
//					Console.WriteLine( "key[{0}]={1}", key, i );
					idx++;
				}
			}
		}

		//@@ public method ----------------------------------------------------
		public ReadOnlyDictionary<string,List<string>> GetOptArgs() {
			var dict = new Dictionary<string,List<string>>();
			foreach ( var d in dict_ ) {
				var list = new List<string>();
				foreach ( var l in GetOptArgs( d.Key ) ) {
					list.Add( l );
				}
				if ( dict.ContainsKey( d.Key ) ) {
					dict[d.Key].AddRange( list );
				} else {
					dict[d.Key] = list;
				}
			}
			return new ReadOnlyDictionary<string,List<string>>( dict );
		}
		public bool TryGetOpt( string key, out IdxPos idxpos ) {
			if ( dict_.TryGetValue( key, out idxpos ) ) {
				return true;
			}
			idxpos = new IdxPos();
			return false;
		}
		public bool TryGetOptArgs( string key, out List<string> args ) {
			args = new List<string>();
			IdxPos idxpos;
			if ( TryGetOpt( key, out idxpos ) ) {
				var beg = idxpos.pos + 1;
				var end = GetNextPosByIdx( idxpos.idx );
				for( int i=beg; i<end; i++ ) {
					args.Add( args_[i] );
				}
				return true;
			}
			return false;
		}
		public List<string> GetOptArgs( string key ) {
			var args = new List<string>();
			if ( TryGetOptArgs( key, out args ) ) {
				return args;
			}
			return args;
		}
	}

	//@@ **********************************************************************
	//@@ class RandomX
	//@@ ・XorShift128P による乱数
	//@@ ・雛形を ChatGPT に作成してもらう
	//@@ **********************************************************************
	public class RandomX {
		//@@ inner -------------------------------------------------------
		private class SplitMix64 {
			private ulong state;
			public SplitMix64(ulong seed) {
				state = seed;
			}
			public ulong Next() {
				ulong z = (state += 0x9E3779B97F4A7C15);
				z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9;
				z = (z ^ (z >> 27)) * 0x94D049BB133111EB;
				return z ^ (z >> 31);
			}
		}

		//@@ field -------------------------------------------------------
		private ulong s0 = 0;
		private ulong s1 = 0;

		//@@ construtor -------------------------------------------------------
		public RandomX() {
			SetSeed();
		}
		public RandomX ( ulong seed ) {
			SetSeed( seed );
		}

		//@@ Set -------------------------------------------------
		public void SetSeed() {
			var tm = (ulong)DateTime.Now.Ticks;
			SetSeed( tm );
		}
		public void SetSeed( ulong seed ) {
			// SplitMix64 で初期化
			SplitMix64 sm64 = new SplitMix64(seed);
			s0 = sm64.Next();
			s1 = sm64.Next();
		}
		//@@ Temper -------------------------------------------------
		public void Temper( int n ) { // SplitMix64 があるので通常は不要
			for ( int i=0; i<n; i++ ) {
				NextUInt();
			}
		}

		//@@ Next ---------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int Next() {
			return NextInt();
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int Next( int limit ) {
			return NextInt( limit );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int Next( int beg, int end ) {
			return NextInt( beg, end );
		}

		//@@ NextInt ---------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int NextInt() {
			return (int)NextUInt();
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int NextInt( int limit ) {
			return (int)( NextUInt() % limit );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int NextInt( int beg, int end ) {
			return (int)( NextUInt() % ( end - beg ) ) + beg;
		}

		//@@ NextULong ---------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public ulong NextULong(){
			ulong x = s0;
			ulong y = s1;
			s0 = y;
			x ^= x << 23;
			s1 = x ^ y ^ (x >> 17) ^ (y >> 26);
			return s1 + y;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public ulong NextULong( ulong limit ) {
			return (ulong)( NextULong() % limit );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public ulong NextULong( ulong beg, ulong end ) {
			return (ulong)( NextULong() % ( end - beg ) ) + beg;
		}

		//@@ NextUInt ---------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public uint NextUInt(){
			return (uint)NextULong();
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public uint NextUInt( uint limit ) {
			return (uint)( NextUInt() % limit );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public uint NextUInt( uint beg, uint end ) {
			return (uint)( NextUInt() % ( end - beg ) ) + beg;
		}

		//@@ NextDouble ---------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double NextDouble() {
			double r = (double)NextUInt();
			return r * 2.3283064365386963e-10;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double NextDouble( double limit ) {
			return NextDouble() * limit;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double NextDouble( double beg, double end ) {
			return NextDouble() * ( end - beg ) + beg;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double NextDoubleExp( double lambda ){ // 指数分布
			return - Math.Log( 1.0 - NextDouble() ) / lambda;
		}

		//@@ NextNormal ---------------------------------------------------------
		public double NextNormal( double m, double v ){
			double x0 = NextDouble();
			double x1 = NextDouble();
			double a = Math.Sqrt( - 2.0 * Math.Log( 1.0 - x0 ) );
			double b = 2.0 * Math.PI * x1;
			double y0 = a * Math.Sin( b );
//			double y1 = a * Math.Cos( b );
			return m + v * y0;
		}

		//@@ Simple ---------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int I() {
			return NextInt();
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int I( int limit ) {
			return NextInt( limit );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int I( int beg, int end ) {
			return NextInt( beg, end );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double P() {
			return NextDouble();
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double P( double limit ) {
			return NextDouble() * limit;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double P( double beg, double end ) {
			return NextDouble() * ( end - beg ) + beg;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public bool B( double p=0.5 ){
			return NextDouble() < p;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double N( double m, double v ){
			return NextNormal( m, v );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double N( double m, double v, double min, double max, int tryNum=8 ){
			var p = N( m, v );
			var cnt = 0;
			while ( cnt<tryNum && ( p<min || max<=p ) ) {
				p = N( m, v );
				cnt++;
			}
			return Math.Max( min, Math.Min( p, max - Double.Epsilon ) );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double Pow( double widthFromZeroToOne, double pow ) {
			return widthFromZeroToOne*(1.0-Math.Pow(P(),1.0/(pow+1)));
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double Exp( double lambda ) {
			return -Math.Log(1.0-P())/lambda;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double Exp( double lambda, double min, double max, int tryNum=8 ){
			var p = Exp( lambda );
			var cnt = 0;
			while ( cnt<tryNum && ( p<min || max<=p ) ) {
				p = Exp( lambda );
				cnt++;
			}
			return Math.Max( min, Math.Min( p, max - Double.Epsilon ) );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int Dice<T>( T[] dts, Func<T,double> func ) {
			var p = P();
			var s = 0.0;
			for ( int i=0; i<dts.Length; i++ ) {
				s += func( dts[i] );
				if ( p < s ) return i;
			}
			return dts.Length-1;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double Pow01( double pow ) {
			return Pow( 1, pow );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public double Exp01( double lambda=5.0 ) {
			return Exp( lambda, 0, 1, tryNum:8 );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int DiceLin( int max ) {
			return (int)(max*(1.0-Math.Sqrt(P())));
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int DicePow( int max, double pow ) {
			return (int)(max*Pow( 1, pow ));
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int DiceExp( int max, double lambda=5.0 ) {
			return (int)(max*Exp( lambda, 0, 1, tryNum:8 ));
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int DiceN( int min, int max, double m, double v ) {
			return (int)Math.Round( N( m, v, min, max, tryNum:8 ) );
		}
	}

	//@@ ***********************************************************************
	//@@ class Shuffle
	//@@ ***********************************************************************
	public class Shuffle {

		//@@ field ------------------------------------------------------------
		public static RandomX rand = new RandomX( 42 );
		private int[] sequence_= new int[0];
		private int cur_ = 0;
		private bool autoShuffle_ = true;

		//@@ constructor ------------------------------------------------------
		public Shuffle() {
			Resize( 1 );
		}
		public Shuffle( int len ) {
			Resize( len );
		}
		public Shuffle( int[] dats ) {
			Resize( dats.Length );
		}

		//@@ property ---------------------------------------------------------
		public int Len { get { return sequence_.Length; } }
		public bool AutoShuffle { get { return autoShuffle_; } set { autoShuffle_ = value; } }
		public int this[int i] { get { return sequence_[i]; } set { sequence_[i] = value; } }

		//@@ private method ---------------------------------------------------
		public void ReShuffle() {
			int size = sequence_.Length;
			for ( int i=0; i<size; i++ ) {
				int r = rand.NextInt( size - i ) + i;
				int n = sequence_[i];
				sequence_[i] = sequence_[r];
				sequence_[r] = n;
			}
			cur_ = 0;
		}

		//@@ public method ----------------------------------------------------
		public void Resize( int[] dats ) {
			if ( Len != dats.Length ) sequence_ = new int[dats.Length];
			for ( int i=0; i<dats.Length; i++ ) {
				sequence_[i] = dats[i];
			}
			cur_ = this.Len;
		}
		public void Resize( int len ) {
			if ( Len != len ) {
				sequence_ = new int[len];
				for ( int i=0; i<len; i++ ) {
					sequence_[i] = i;
				}
			}
			cur_ = this.Len;
		}
		public int Get() {
			if ( autoShuffle_ && ( sequence_.Length <= cur_ ) ) {
				ReShuffle();
			}
			return sequence_[cur_++];
		}

		//@@ static method ----------------------------------------------------
		public static void All<T>( T[] arr ) {
			var size = arr.Length;
			for ( int i=0; i<size; i++ ) {
				int r = rand.NextInt( size - i ) + i;
				var n = arr[i];
				arr[i] = arr[r];
				arr[r] = n;
			}
		}
		public static void All<T>( List<T> ls ) {
			var size = ls.Count;
			for ( int i=0; i<size; i++ ) {
				int r = rand.NextInt( size - i ) + i;
				var n = ls[i];
				ls[i] = ls[r];
				ls[r] = n;
			}
		}
		public static void All<T>( Span<T> sp ) {
			var size = sp.Length;
			for ( int i=0; i<size; i++ ) {
				int r = rand.NextInt( size - i ) + i;
				var n = sp[i];
				sp[i] = sp[r];
				sp[r] = n;
			}
		}
		public static void Range<T>( T[] arr, int beg, int len ) {
			for ( int i=0; i<len; i++ ) {
				int r = rand.NextInt( len - i ) + i + beg;
				var n = arr[beg+i];
				arr[beg+i] = arr[r];
				arr[r] = n;
			}
		}
		public static void Range<T>( List<T> ls, int beg, int len ) {
			for ( int i=0; i<len; i++ ) {
				int r = rand.NextInt( len - i ) + i + beg;
				var n = ls[beg+i];
				ls[beg+i] = ls[r];
				ls[r] = n;
			}
		}
		public static void Range<T>( Span<T> sp, int beg, int len ) {
			for ( int i=0; i<len; i++ ) {
				int r = rand.NextInt( len - i ) + i + beg;
				var n = sp[beg+i];
				sp[beg+i] = sp[r];
				sp[r] = n;
			}
		}
		public static void Swap1( List<int> ls ) {
			var i = rand.NextInt( ls.Count );
			var j = i;
			while ( i==j ) {
				j = rand.NextInt( ls.Count );
			}
			var n = ls[i];
			ls[i] = ls[j];
			ls[j] = n;
		}
	}

	//@@ ******************************************************************************************
	//@@ class Sort
	//@@ ******************************************************************************************
	public static class Sort {

		//@@ RadixSort ------------------------------------------------------------
		public static void RadixSort( Span<int> a ) {
			// バケツを用意
			var bucket = new List<int>[256];
			for ( int d=0,logR=0; d<4; ++d,logR+=8 ) {
				// バケツに値を入れる
				for ( int i=0; i<a.Length; ++i ) {
					int key = (a[i]>>logR)&255; // a[i] を256進 d 桁目だけを取り出す。
					if ( bucket[key]==null) bucket[key] = new List<int>();
					bucket[key].Add( a[i] );
				}
				// バケツ中の値の結合
				for ( int j=0, i=0; j<bucket.Length; j++ ) {
					if ( bucket[j]!=null ) {
						foreach ( int val in bucket[j] ) {
							a[i++] = val;
						}
					}
				}
				// バケツを一度空にする
				for ( int j=0; j<bucket.Length; j++ ) {
					if ( bucket[j]==null ) continue;
					bucket[j].Clear();
				}
			}
		}
		public static void RadixSort<T>( Span<T> a, Func<T,int> fKey ) {
			// バケツを用意
			var bucket = new List<T>[256];
			for ( int d=0,logR=0; d<4; ++d,logR+=8 ) {
				// バケツに値を入れる
				for ( int i=0; i<a.Length; ++i ) {
					int key = (fKey(a[i])>>logR)&255; // a[i] を256進 d 桁目だけを取り出す。
					if ( bucket[key]==null) bucket[key] = new List<T>();
					bucket[key].Add( a[i] );
				}
				// バケツ中の値の結合
				for ( int j=0, i=0; j<bucket.Length; j++ ) {
					if ( bucket[j]!=null ) {
						foreach ( var val in bucket[j] ) {
							a[i++] = val;
						}
					}
				}
				// バケツを一度空にする
				for ( int j=0; j<bucket.Length; j++ ) {
					if ( bucket[j]==null ) continue;
					bucket[j].Clear();
				}
			}
		}
		public unsafe static void RadixSortOpt<T>( T[] a0, Func<T,int> fKey ) {
			// バケツを用意
			var a1 = new T[a0.Length];
			var keys = stackalloc int[a0.Length];
			var cnts = stackalloc int[256];
			var poss = stackalloc int[256];
			for ( int d=0,logR=0; d<4; ++d,logR+=8 ) {
				for ( int j=0; j<256; j++ ) cnts[j] = 0;
				// バケツに値を入れる個数カウント
				for ( int i=0; i<a0.Length; ++i ) {
					int k = (fKey(a0[i])>>logR)&255; // a[i] を256進 d 桁目だけを取り出す。
					keys[i] = k;
					cnts[k]++;
				}
				poss[0] = 0;
				for ( int i=0; i<255; ++i ) {
					poss[i+1] = poss[i] + cnts[i];
				}

				// バケツに値を入れる
				for ( int i=0; i<a0.Length; ++i ) {
					var j = keys[i];
					a1[poss[j]] = a0[i];
					poss[j]++;
				}
				(a0,a1)=(a1,a0);
			}
		}

		//@@ SelectNthQuick<T> ------------------------------------------------------------
		// Quickselect を使ってK番目を得る。K未満はspan[K]以下のソートされていない要素、K+1以上はspan[K]以上のソートされていない要素が入る
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		private static int Partition<T>( Span<T> span, int left, int right) where T : IComparable<T> {
			T pivot = span[right];
			int i = left;
			for (int j = left; j < right; j++) {
				if (span[j].CompareTo(pivot) <= 0) {
					var a = span[i];
					span[i] = span[j];
					span[j] = a;
					i++;
				}
			}
			var t = span[i];
			span[i] = span[right];
			span[right] = t;
			return i;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		private static T Quickselect<T>( Span<T> span, int left, int right, int N) where T : IComparable<T> {
			while (left < right) {
				int pivotIndex = Partition(span, left, right);
				if (N == pivotIndex) {
					return span[pivotIndex];
				} else if (N < pivotIndex) {
					right = pivotIndex - 1;
				} else {
					left = pivotIndex + 1;
				}
			}
			return span[left];
		}
		public static T SelectNthQuick<T>( Span<T> span, int N) where T : IComparable<T> {
			if (span == null || N < 0 || N >= span.Length) {
				throw new ArgumentOutOfRangeException();
			}
			return Quickselect(span, 0, span.Length - 1, N);
		}

		//@@ SelectNthQuick<T,Key> ------------------------------------------------------------
		// Quickselect を使ってK番目を得る。K未満はspan[K]以下のソートされていない要素、K+1以上はspan[K]以上のソートされていない要素が入る
		// K番目を得る。K未満はspan[K]以下のソートされていない要素、K+1以上はspan[K]以上のソートされていない要素が入る
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		private static int Partition<T,Key>( Span<T> span, int left, int right, Func<T,Key> fKey ) where Key : INumber<Key> {
			T pivot = span[right];
			int i = left;
			for (int j = left; j < right; j++) {
				if (fKey(span[j])<=fKey(pivot)) {
					var a = span[i];
					span[i] = span[j];
					span[j] = a;
					i++;
				}
			}
			var t = span[i];
			span[i] = span[right];
			span[right] = t;
			return i;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		private static T Quickselect<T,Key>( Span<T> span, int left, int right, int N, Func<T,Key> fKey ) where Key : INumber<Key> {
			while (left < right) {
				int pivotIndex = Partition(span, left, right, fKey);
				if (N == pivotIndex) {
					return span[pivotIndex];
				} else if (N < pivotIndex) {
					right = pivotIndex - 1;
				} else {
					left = pivotIndex + 1;
				}
			}
			return span[left];
		}
		public static T SelectNthQuick<T,Key>( Span<T> span, int N, Func<T,Key> fKey ) where Key : INumber<Key> {
			if (span == null || N < 0 || N >= span.Length) {
				throw new ArgumentOutOfRangeException();
			}
			return Quickselect(span, 0, span.Length - 1, N, fKey );
		}
	}

	//@@ ******************************************************************************************
	//@@ class ObjectPoolCls
	//@@ ******************************************************************************************
	public class ObjectPoolCls<T> where T : class, new() {

		//@@ field ------------------------------------------------------------
		private readonly T[] _items;
		private int _currentIndex;
		private readonly Func<T> _objectGenerator;
		private readonly Action<T> _resetAction;
		private int cntAlloc = 0;
		private int cntFree = 0;
		private int cntNew = 0;
		private int cntDel = 0;
		private int cntMax = 0;

		//@@ constructor ------------------------------------------------------------
		public ObjectPoolCls(Func<T> objectGenerator, bool isPreInit=false, int isPreInitSize=4*1024, int initialCapacity=128*1024, Action<T> resetAction=null) {
			if (initialCapacity <= 0) {
				throw new ArgumentOutOfRangeException(nameof(initialCapacity));
			}
			_objectGenerator = objectGenerator ?? throw new ArgumentNullException(nameof(objectGenerator));
			_resetAction = resetAction;
			_items = new T[initialCapacity];
			_currentIndex = 0;
			if ( isPreInit ) PreInitialize( isPreInitSize );// プール初期化時にオブジェクトを事前生成（オプション）
		}

		// 必要に応じてプールを事前初期化
		public void PreInitialize( int isPreInitSize ) {
			var size = Math.Min( isPreInitSize, _items.Length );
			for (int i = 0; i < size; i++) {
				cntNew++;
				_items[i] = _objectGenerator();
			}
			_currentIndex = size;
		}

		//@@ property ------------------------------------------------------------
		public int Count => _currentIndex;
		public int Capacity => _items.Length;

		//@@ Tool ------------------------------------------------------------
		public void Clear() {
			for (int i = 0; i < _currentIndex; i++) {
				if (_items[i] is IDisposable disposable) {
					cntDel++;
					disposable.Dispose();
				}
				_items[i] = null;
			}
			_currentIndex = 0;
		}

		//@@ Alloc ------------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public T Alloc() {
			cntAlloc++;
			if (_currentIndex <= 0) {
				cntNew++;
				return _objectGenerator();
			}
			_currentIndex--;
			var item = _items[_currentIndex];
			_items[_currentIndex] = null; // GCヘルプのために参照をクリア
			return item!;
		}

		//@@ Free ------------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public bool Free(T item) {
			if (item == null) {
				throw new ArgumentNullException(nameof(item));
			}
			cntFree++;
			if (_currentIndex >= _items.Length) {
				cntDel++;
				if (item is IDisposable disposable) {
					disposable.Dispose();
				}
				return false;
			}
			_resetAction?.Invoke(item);
			_items[_currentIndex] = item;
			_currentIndex++;
			cntMax = Math.Max( cntMax, _currentIndex );
			return true;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public void Free(Span<T> items) {
			foreach (var item in items) {
				if (item != null) {
					Free(item);
				}
			}
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public void Free(T[] items) {
			foreach (var item in items) {
				if (item != null) {
					Free(item);
				}
			}
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public void Free(List<T> items) {
			foreach (var item in items) {
				if (item != null) {
					Free(item);
				}
			}
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public void Free(IEnumerable<T> items) {
			foreach (var item in items) {
				if (item != null) {
					Free(item);
				}
			}
		}


		//@@ Debug ------------------------------------------------------------
		public T[] DebugCopyAllObjects() {
			var result = new T[_currentIndex];
			Array.Copy(_items, result, _currentIndex);
			return result;
		}

		//@@ Print ------------------------------------------------------------
		public string PrintStr( string msg="" ) {
			var sw = new StringWriter();
			sw.WriteLine($"#ObjectPoolCls<{typeof(T)}> {msg}");
			sw.WriteLine($"#    cntAlloc = {cntAlloc:#,0}");
			sw.WriteLine($"#    cntFree  = {cntFree:#,0}");
			sw.WriteLine($"#    cntNew   = {cntNew:#,0}");
			sw.WriteLine($"#    cntDel   = {cntDel:#,0}");
			sw.WriteLine($"#    cntMax   = {cntMax:#,0}");
			return sw.ToString();
		}
		public void Print( string msg="" ) {
			std.Print(PrintStr(msg));
		}
	}

	//@@ ******************************************************************************************
	//@@ class DisposableObjectPoolCls
	//@@ claude3に相談して改良。使い捨てバージョン
	//@@ ******************************************************************************************
	public class DisposableObjectPoolCls<T> where T : class {

		//@@ field ------------------------------------------------------------
		private readonly T[] _items;
		private int _currentIndex;
		private readonly Func<T> _objectGenerator;
		private int cntAlloc = 0;
		private int cntFree = 0;
		private int cntNew = 0;
		private int cntDel = 0;
		private int cntMax = 0;

		//@@ constructor ------------------------------------------------------------
		public DisposableObjectPoolCls(Func<T> objectGenerator, bool isPreInit=true, int isPreInitSize=32*1024, int initialCapacity=128*1024 ) {
			if (initialCapacity <= 0) {
				throw new ArgumentOutOfRangeException(nameof(initialCapacity));
			}
			_objectGenerator = objectGenerator ?? throw new ArgumentNullException(nameof(objectGenerator));
			_items = new T[initialCapacity];
			_currentIndex = 0;
			if ( isPreInit ) PreInitialize( isPreInitSize );// プール初期化時にオブジェクトを事前生成（オプション）
		}

		// 必要に応じてプールを事前初期化
		public void PreInitialize( int isPreInitSize ) {
			var size = Math.Min( isPreInitSize, _items.Length );
			for (int i = 0; i < size; i++) {
				cntNew++;
				_items[i] = _objectGenerator();
			}
			_currentIndex = 0;
		}

		//@@ property ------------------------------------------------------------
		public int Count => _currentIndex;
		public int Capacity => _items.Length;

		//@@ Tool ------------------------------------------------------------
		public void Clear() {
			for (int i = 0; i < _currentIndex; i++) {
				if (_items[i] is IDisposable disposable) {
					cntDel++;
					disposable.Dispose();
				}
				_items[i] = null;
			}
			_currentIndex = 0;
		}
		public void ClearCnt() {
			cntAlloc = 0;
			cntFree = 0;
			cntNew = 0;
			cntDel = 0;
		}

		//@@ Alloc ------------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public T Alloc() {
			cntAlloc++;
			if ( _items.Length <= _currentIndex ) {
				cntNew++;
				return _objectGenerator();
			}
			var item = _items[_currentIndex];
			if ( item == null ) {
				cntNew++;
				item = _objectGenerator();
				_items[_currentIndex] = item;
			}
			_currentIndex++;
			cntMax = Math.Max( cntMax, _currentIndex );
			return item!;
		}

		//@@ Free ------------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public void Free() {
			cntFree += _currentIndex;
			_currentIndex = 0;
		}

		//@@ Debug ------------------------------------------------------------
		public T[] DebugCopyAllObjects() {
			var result = new T[_currentIndex];
			Array.Copy(_items, result, _currentIndex);
			return result;
		}

		//@@ Print ------------------------------------------------------------
		public string PrintStr( string msg="" ) {
			var sw = new StringWriter();
			sw.WriteLine($"#DisposableObjectPoolCls<{typeof(T)}> {msg}");
			sw.WriteLine($"#    cntAlloc = {cntAlloc:#,0}");
			sw.WriteLine($"#    cntFree  = {cntFree:#,0}");
			sw.WriteLine($"#    cntNew   = {cntNew:#,0}");
			sw.WriteLine($"#    cntDel   = {cntDel:#,0}");
			sw.WriteLine($"#    cntMax   = {cntMax:#,0}");
			return sw.ToString();
		}
		public void Print( string msg="" ) {
			std.Print(PrintStr(msg));
		}
	}

	//@@ ******************************************************************************************
	//@@ class QueuePow2<T>
	//@@ ・長さが２の指数乗の固定長Queue。エラー処理を削除すれば、標準のQueueの倍速ぐらいのスピード
	//@@ ******************************************************************************************
	public class QueuePow2<T> : IEnumerable<T> {
		//@@ field ------------------------------------------------------------
		private T[] _buffer;
		private int _head;
		private int _tail;
		private readonly int _capacity;
		private readonly int _powerOf2;
		private readonly int _mask;

		//@@ ctor ------------------------------------------------------------
		public QueuePow2(int size) {
			var bitLen = CalcBitLen( size );
			_powerOf2 = bitLen;
			_mask = (1<<bitLen)-1;
			_capacity = 1<<bitLen;
			_buffer = new T[_capacity];
			_head = 0;
			_tail = 0;
		}

		public IEnumerator<T> GetEnumerator() {
			for ( int i=_tail; i<_head; i++ ) {
				yield return _buffer[i];
			}
		}
		IEnumerator IEnumerable.GetEnumerator() {
			for ( int i=_tail; i<_head; i++ ) {
				yield return _buffer[i];
			}
		}

		//@@ property ------------------------------------------------------------
		public int Count => _head-_tail;
		public bool Empty => _head==_tail;
		public bool Exists => _head!=_tail;
		public bool Full => _head-_capacity==_tail;
		public T this[int i] => _buffer[(_head+i)&_mask];

		//@@ method ------------------------------------------------------------
		public void Clear() {
			_head = 0;
			_tail = 0;
		}
		public static int CalcBitLen( int n ) {
			var bit = 0;
			var v = 1;
			while ( v < n ) {
				v <<= 1;
				bit++;
			}
			return bit;
		}
		public void Push(T item) {
//			if (Full) throw new InvalidOperationException($"QueuePow2.Push > Queue is full > cap {_capacity} count {Count} tail {_tail} head {_head}" );
			_buffer[_head&_mask] = item;
			_head++;
		}

		public T Pop() {
//			if (Empty) throw new InvalidOperationException($"QueuePow2.Pop > Queue is empty>  cap {_capacity} count {Count} tail {_tail} head {_head}");
			T item = _buffer[_tail&_mask];
			_tail++;
			return item;
		}

		public T Peek() {
//			if (Empty) throw new InvalidOperationException($"QueuePow2.Peek > Queue is empty >  cap {_capacity} count {Count} tail {_tail} head {_head}");
			return _buffer[_tail];
		}
	}

	//@@ **********************************************************************
	//@@ class Temperature
	//@@ ネットで調べた詰め合わせ
	//@@ **********************************************************************
	public class Temperature {
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static double Geometric( int turn, double alpha ) { // 一般的に 0.8<alpha<0.99 らしい
			return Math.Pow( alpha, turn );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static double Logarithmic( int turn, double preT, double c ) { // 一般的に 1.0<c らしい
			return preT / ( 1.0 + c * Math.Log( 1.0 + turn ) );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static double Polynomial( int turn, double p ) { // 一般的に 1<p<2 らしい
			return Math.Pow( ( 1.0 + turn ), -p );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static double Linear( int turn, double delta ) {
			return Math.Max( 0.0, 1.0 - turn * delta );
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static double Half( int turn, int halfT ) {
			return 1.0 * halfT / ( halfT + turn );
		}
		public static double[] GenGeometrics( int num, double min, double max ) {
			var ts = new double[num];
			var r = Math.Pow( max/min, 1.0/(num-1) );
			var g = r;
			ts[0] = min;
			for ( int i=1; i<num; i++ ) {
				ts[i] = min * g;
				g *= r;
			}
			return ts;
		}
	}

	//@@ **********************************************************************
	//@@ class CntLog
	//@@ **********************************************************************
	public class CntLog {
		public int[] logCnt = null;
		public int[] logAcCnt = null;
		public int[] logBestCnt = null;
		public int[] logSwapAcCnt = null;
		public int[] logSwapCnt = null;
		public CntLog( int size ) {
			Resize( size );
			Reset();
		}
		public void Resize( int size ) {
			logCnt = new int[size];
			logAcCnt = new int[size];
			logBestCnt = new int[size];
			logSwapAcCnt = new int[size];
			logSwapCnt = new int[size];
		}
		public void Reset() {
			for ( int i=0; i<logCnt.Length; i++ ) logCnt[i] = 0;
			for ( int i=0; i<logAcCnt.Length; i++ ) logAcCnt[i] = 0;
			for ( int i=0; i<logBestCnt.Length; i++ ) logBestCnt[i] = 0;
			for ( int i=0; i<logSwapCnt.Length; i++ ) logSwapCnt[i] = 0;
			for ( int i=0; i<logSwapAcCnt.Length; i++ ) logSwapAcCnt[i] = 0;
		}
		public void Clear() {
			Reset();
		}
		public string PrintStr() {
			var mCnt = String.Join(' ',logCnt.Select(i=>i.ToString()).ToList());
			var mBes = String.Join(' ',logBestCnt.Select(i=>i.ToString()).ToList());
			var mAcr = "";
			var mSwp = "";
			for ( int i=0; i<logAcCnt.Length; i++ ) mSwp += $"{1.0*logSwapAcCnt[i]/logSwapCnt[i]:F2} ";
			for ( int i=0; i<logAcCnt.Length; i++ ) mAcr += $"{1.0*logAcCnt[i]/logCnt[i]:F3} ";
			return $"cnt {logCnt[0]} ac {mAcr} best {mBes} swap {mSwp}";
		}
	}

	//@@ **********************************************************************
	//@@ class SimulatedAnnealing
	//@@ **********************************************************************
	public class SimulatedAnnealing {

		//@@ static -------------------------------------------------------
		public static Stopwatch sw = new Stopwatch();
		public static RandomX rand = new RandomX( 42 );

		//@@ constructor -------------------------------------------------------
		public SimulatedAnnealing() {
		}

		//@@ RunHC -------------------------------------------------------
		//@@ 単純な山登り
		public static List<Sample> RunHC( double timeout, List<Sample> staSms, bool inplace ) {
			var poolT = Transition.pool;

			var wrkSms = new Sample[staSms.Count];

			// 初期ステートとベスト作成
			for ( int i=0; i<staSms.Count; i++ ) {
				wrkSms[i] = ( inplace ) ? staSms[i] : staSms[i].Dup();
			}

			// メインループ
			sw.Restart();
			var logAcCnt = 0;
			var loop = 0;
			var nowMsec = (double)sw.ElapsedMilliseconds;
			while ( true ) {
				var loopCycle = 0x3f;
				if ( (loop&loopCycle)==0 ) {
					nowMsec = sw.ElapsedMilliseconds;
					if ( timeout < nowMsec ) break;
//					std.Print($"msec[{(int)nowMsec}] acCnt {logAcCnt:F3}");
					logAcCnt = 0;
				}

				// 各Sampleを遷移していく
				for ( int i=0; i<wrkSms.Length; i++ ) {
					var sample = wrkSms[i];

					// Transitionの遷移判定
					var tra = sample.NextTra();
					if ( sample.score>tra.score ) {
						// Sampleを遷移させる
						sample.ApplyTra( tra );
						logAcCnt++;
					}
				}

				// Transitionのインスタンスをプールに戻す
				poolT.Free();
				loop++;
			}
			var ansSms = new Sample[staSms.Count];
			for ( int i=0; i<wrkSms.Length; i++ ) {
				ansSms[i] = ( inplace ) ? wrkSms[i] : wrkSms[i].Dup();
			}
			std.Print($"loop[{loop:#,0}] -> Finish");
			return ansSms.ToList();
		}

		//@@ RunMI -------------------------------------------------------
		//@@ 時間減衰温度スケジュール
		public static List<Sample> RunMI( double timeout, List<Sample> staSms ) {
			var poolT = Transition.pool;

			var energyGain = Prm.prmSaMiEnergyGain;// 一般的に低温時でAC率が10%から30%ぐらい、高温時で90%ぐらいになるようにする。当然問題毎に調整必須。
			var alpha      = Prm.prmSaMiAlpha;

			var logCnt = 0;
			var logAcCnt = 0;
			var logBestCnt = 0;

			var wrkSms = new Sample[staSms.Count];
			var bestSms = new Sample[staSms.Count];

			// 初期ステートとベスト作成
			for ( int i=0; i<staSms.Count; i++ ) {
				wrkSms[i] = staSms[i];
				bestSms[i] = staSms[i].Dup();
			}

			// メインループ
			sw.Restart();
			var loop = 0;
			var nowMsec = (double)sw.ElapsedMilliseconds;
			var temperature = 1.0; // 0.0 <= t <= 1.0; とする。
			while ( true ) {
				var loopCycle = 0x3f;
				if ( (loop&loopCycle)==0 ) {
					nowMsec = sw.ElapsedMilliseconds;
					if ( timeout < nowMsec ) break;
				}

				var updateCycle = 0x3ff;
				if ( (loop&updateCycle)==0 ) {
					// 温度のスケジュール
					var preT = temperature;
					var msec = (int)nowMsec;

//					temperature = 1.0;
					temperature = Temperature.Geometric( msec, alpha:alpha ); // 一般的に 0.8<alpha<0.99 らしい
//					temperature = Temperature.Logarithmic( msec, preT:preT, c:5.0e-3 ); // 一般的に 1.0<c らしい
//					temperature = Temperature.Polynomial( msec, p:1.0 ); // 一般的に 1<p<2 らしい
//					temperature = Temperature.Linear( msec, delta:1.0e-3 );
//					temperature = Temperature.Half( msec, halfT:100 );

//					std.Print($"msec[{msec,4}] temperature {temperature:F3} cnt {logCnt} acRate {1.0*logAcCnt/logCnt:F3} bestCnt {logBestCnt}");
					logCnt = 0;
					logAcCnt = 0;
					logBestCnt = 0;
				}

				// 各Sampleを遷移していく
				for ( int i=0; i<wrkSms.Length; i++ ) {
					var wrkSm= wrkSms[i];

					// Transitionの遷移判定
					wrkSm.temperature = temperature;
					var tra = wrkSm.NextTra();
					var energy = energyGain * ( wrkSm.score - tra.score );
//					std.Print($"e {energy} p {Math.Exp( energy/temperature )}");
					if ( 0<energy || rand.NextDouble()<Math.Exp( energy/temperature ) ) {
						// Sampleを遷移させる
						wrkSm.ApplyTra( tra );
						logAcCnt++;

						// ベストかどうかの判定
						if ( bestSms[i].score>wrkSm.score ) {
							bestSms[i].CopyFromForSA( wrkSm );
//							Out.WriteLine($"loop[{loop}] best[{i}] -> {bestSms[i].score} {bestSms[i].val}");
							logBestCnt++;
						}
					}
					logCnt++;
				}

				// Transitionのインスタンスをプールに戻す
				poolT.Free();

				loop++;
			}
			std.Print($"loop[{loop:#,0}] -> Finish");
			return bestSms.ToList();
		}
	}

	//@@ **********************************************************************
	//@@ class SegmentQuadTree
	//@@ chatGTPにサンプルコードを作成してもらう
	//@@ 色々と怪しい実装だったので修正（時間溶けた。。。）。総当たりの数倍早かったが結局使わず。。。
	//@@ **********************************************************************
	public class SegmentQuadTree {
		public struct Segment : IEquatable<Segment> {
 			public readonly int X0, Y0, X1, Y1;
			public readonly AABB mbr;
			public Segment( int x0, int y0, int x1, int y1 ) {
				X0 = x0; Y0 = y0; X1 = x1; Y1 = y1;
				mbr = new AABB(Math.Min(x0,x1),Math.Max(x0,x1),Math.Min(y0,y1),Math.Max(y0,y1));
			}
			public AABB BoundingBox => mbr;
			public bool BoundingBoxOverlap( Segment o ) => mbr.Intersects( o.mbr );

			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			public static int Cross( int x0, int y0, int x1, int y1, int x2, int y2 ) {
				var v = ( x1 - x0 ) * ( y2 - y0 ) - ( y1 - y0 ) * ( x2 - x0 );
				return Math.Sign( v );
			}
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			public static bool Intersects( int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3 ) {
				var c0 = Cross( x0, y0, x1, y1, x2, y2 );
				var c1 = Cross( x0, y0, x1, y1, x3, y3 );
				if ( 0<c0*c1 ) return false;
				var c2 = Cross( x2, y2, x3, y3, x0, y0 );
				var c3 = Cross( x2, y2, x3, y3, x1, y1 );
				return ( c2*c3<=0 );
			}
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			public bool Intersects( Segment o ) {
				if (
					( X0==o.X0 && Y0==o.Y0 ) ||
					( X0==o.X1 && Y0==o.Y1 ) ||
					( X1==o.X0 && Y1==o.Y0 ) ||
					( X1==o.X1 && Y1==o.Y1 )
				) return false;
				return Intersects( 
					this.X0,this.Y0,
					this.X1,this.Y1,
					o.X0,o.Y0,
					o.X1,o.Y1
				);
			}
			public bool Equals( Segment o ) {
				return (X0==o.X0 && Y0==o.Y0 && X1==o.X1 && Y1==o.Y1 );
			}
		};

		public record AABB( int minX, int maxX, int minY, int maxY ) {
			public bool Intersects( AABB other ) => !( maxX<other.minX || minX>other.maxX || maxY<other.minY || minY>other.maxY );
			public bool Contains( int x,int y ) => minX<=x && x<=maxX && minY<=y && y<= maxY;
		}

		public class QuadTree {
			private const int MaxDepth = 3;
			private const int Capacity = 24;
			private readonly AABB bounds;
			private List<Segment> segments = new();
			private int depth;
			private bool divided = false;
			private QuadTree nw, ne, sw, se;
			public QuadTree(AABB bounds, int dp=0 ) {
				this.bounds = bounds;
				this.depth = dp;
			}
			public AABB Bounds => bounds;
			public IEnumerable<Segment> Enumerate() {
				foreach (var s in segments) {
					yield return s;
				}
				if (divided) {
					if ( nw!=null ) foreach (var s in nw.Enumerate()) yield return s;
					if ( ne!=null ) foreach (var s in ne.Enumerate()) yield return s;
					if ( sw!=null ) foreach (var s in sw.Enumerate()) yield return s;
					if ( se!=null ) foreach (var s in se.Enumerate()) yield return s;
				}
			}
			public void Insert( Segment s, int depth=0 ) {
				if ( !bounds.Intersects( s.BoundingBox ) ) {
					return;
				}
				if ( MaxDepth<=depth || ( segments.Count<Capacity && !divided ) ) {
					segments.Add(s);
				} else {
					if ( !divided ) Subdivide();
					nw!.Insert( s,depth+1 );
					ne!.Insert( s,depth+1 );
					sw!.Insert( s,depth+1 );
					se!.Insert( s,depth+1 );
				}
			}
			public void Clear() {
				segments.Clear();
				if ( divided ) {
					nw!.Clear();
					ne!.Clear();
					sw!.Clear();
					se!.Clear();
				}
			}
			public bool Remove( Segment s ) {
				if ( !bounds.Intersects( s.BoundingBox ) ) {
					return false;
				}
				bool ret = segments.Remove(s);
				if (divided) {
					ret |= nw!.Remove(s);
					ret |= ne!.Remove(s);
					ret |= sw!.Remove(s);
					ret |= se!.Remove(s);
				}
				return ret;
			}
			public bool Query( Segment target ) {
				var (x0,y0,x1,y1) = (target.X0,target.Y0,target.X1,target.Y1);
				if ( !bounds.Intersects( target.BoundingBox ) ) {
					return false;
				}
				foreach ( var s in segments ) {
					if (
						( s.X0==target.X0 && s.Y0==target.Y0 ) ||
						( s.X0==target.X1 && s.Y0==target.Y1 ) ||
						( s.X1==target.X0 && s.Y1==target.Y0 ) ||
						( s.X1==target.X1 && s.Y1==target.Y1 )
					) continue;
					if ( s.Intersects( target ) ) return true;
				}
				if (divided) {
					return nw!.Query(target) || ne!.Query(target) || sw!.Query(target) || se!.Query(target);
				}
				return false;
			}
			private void Subdivide() {
				int midX = (bounds.minX + bounds.maxX) / 2;
				int midY = (bounds.minY + bounds.maxY) / 2;
				nw = new QuadTree( new AABB( bounds.minX,        midX,        midY, bounds.maxY ), depth + 1 );
				ne = new QuadTree( new AABB(        midX, bounds.maxX,        midY, bounds.maxY ), depth + 1 );
				sw = new QuadTree( new AABB( bounds.minX,        midX, bounds.minY,        midY ), depth + 1 );
				se = new QuadTree( new AABB(        midX, bounds.maxX, bounds.minY,        midY ), depth + 1 );
				divided = true;
				foreach ( var s in segments ) {
					Insert(s);
				}
				segments.Clear();
			}
		}
		private QuadTree quad;
		public SegmentQuadTree(int x0, int x1, int y0, int y1 ) {
			quad = new QuadTree(new AABB(x0,x1,y0,y1));
		}
		public SegmentQuadTree Dup() {
			var (x0,x1,y0,y1) = quad.Bounds;
			var o = new SegmentQuadTree(x0,x1,y0,y1);
			o.CopyFrom( this );
			return o;
		}
		public void CopyFrom(SegmentQuadTree o) {
			this.Clear();
			foreach (var seg in o.quad.Enumerate()) {
				this.Add(seg);
			}
		}
		public void Clear() => quad.Clear();
		public void Add(Segment s) => quad.Insert(s);
		public void Add(int x0, int y0, int x1, int y1 ) {
			quad.Insert(new Segment(x0,y0,x1,y1));
		}
		public void Add((int,int,int,int) seg) {
			quad.Insert(new Segment(seg.Item1,seg.Item2,seg.Item3,seg.Item4));
		}
		public void Remove(Segment s) {
			var ret = quad.Remove(s);
			if ( !ret ) {
				std.Throw( $"QuadTree.Remove > no segment error" );
			}
		}
		public void Remove(int x0, int y0, int x1, int y1 ) {
			quad.Remove(new Segment(x0,y0,x1,y1));
		}

		public bool Intersects(int x0, int y0, int x1, int y1 ) {
			return Intersects( new Segment(x0,y0,x1,y1) );
		}
		public bool Intersects( Segment s ) {
			return quad.Query(s);
		}
	}

	//@@ **********************************************************************
	//@@ class SegmentRTree
	//@@ chatGTPにサンプルコードを作成してもらう
	//@@ 色々と怪しい実装だったので修正（時間溶けたその２。。。）。そして使わず。。。
	//@@ **********************************************************************
	public class SegmentRTree {
		public struct MBR {
			public int MinX, MinY, MaxX, MaxY;
			public MBR( int minX, int minY, int maxX, int maxY ) {
				MinX = minX;
				MinY = minY;
				MaxX = maxX;
				MaxY = maxY;
			}
			public bool Overlaps( MBR o ) {
				return !( MaxX<o.MinX || MinX>o.MaxX || MaxY<o.MinY || MinY>o.MaxY );
			}
			public void ExpandToInclude(MBR o) {
				MinX = Math.Min( MinX, o.MinX );
				MinY = Math.Min( MinY, o.MinY );
				MaxX = Math.Max( MaxX, o.MaxX );
				MaxY = Math.Max( MaxY, o.MaxY );
			}
			public static MBR Combine( MBR a, MBR b ) {
				return new MBR(
					Math.Min( a.MinX, b.MinX ),
					Math.Min( a.MinY, b.MinY ),
					Math.Max( a.MaxX, b.MaxX ),
					Math.Max( a.MaxY, b.MaxY )
				);
			}
		}

		public struct Segment : IEquatable<Segment> {
 			public readonly int X0, Y0, X1, Y1;
			public readonly MBR mbr;
			public Segment( int x0, int y0, int x1, int y1 ) {
				X0 = x0; Y0 = y0; X1 = x1; Y1 = y1;
				mbr = new MBR(Math.Min(x0,x1),Math.Min(y0,y1),Math.Max(x0,x1),Math.Max(y0,y1));
			}
			public MBR GetMBR() => mbr;
			public bool BoundingBoxOverlap( Segment o ) => mbr.Overlaps( o.mbr );

			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			public static int Cross( int x0, int y0, int x1, int y1, int x2, int y2 ) {
				var v = ( x1 - x0 ) * ( y2 - y0 ) - ( y1 - y0 ) * ( x2 - x0 );
				return Math.Sign( v );
			}
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			public static bool Intersects( int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3 ) {
				var c0 = Cross( x0, y0, x1, y1, x2, y2 );
				var c1 = Cross( x0, y0, x1, y1, x3, y3 );
				if ( 0<c0*c1 ) return false;
				var c2 = Cross( x2, y2, x3, y3, x0, y0 );
				var c3 = Cross( x2, y2, x3, y3, x1, y1 );
				return ( c2*c3<=0 );
			}
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			public bool Intersects( Segment o ) {
				if (
					( X0==o.X0 && Y0==o.Y0 ) ||
					( X0==o.X1 && Y0==o.Y1 ) ||
					( X1==o.X0 && Y1==o.Y0 ) ||
					( X1==o.X1 && Y1==o.Y1 )
				) return false;
				return Intersects( 
					this.X0,this.Y0,
					this.X1,this.Y1,
					o.X0,o.Y0,
					o.X1,o.Y1
				);
			}
			public bool Equals( Segment o ) {
				return (X0==o.X0 && Y0==o.Y0 && X1==o.X1 && Y1==o.Y1 );
			}
		};

		public class RTreeEntry {
			public MBR Bounds;
			public Segment Segment;
			public RTreeEntry(Segment segment) {
				Segment = segment;
				Bounds = segment.GetMBR();
			}
		}

		public class RTreeNode {
			public const int MaxEntries = 64;
			public bool IsLeaf;
			public List<RTreeEntry> Entries;
			public List<RTreeNode> Children;
			public MBR Bounds;
			public RTreeNode(bool isLeaf) {
				IsLeaf = isLeaf;
				Entries = ( isLeaf ) ? new List<RTreeEntry>() : null;
				Children = ( isLeaf ) ? null : new List<RTreeNode>();
			}
			public void RecalculateBounds() {
				if (IsLeaf) {
					Bounds = Entries[0].Bounds;
					foreach (var e in Entries)
						Bounds.ExpandToInclude(e.Bounds);
				} else {
					Bounds = Children[0].Bounds;
					foreach (var c in Children) {
						Bounds.ExpandToInclude(c.Bounds);
					}
				}
			}
		}
		public class RTree {
			private RTreeNode root = new(true);
			public void Insert(Segment segment) {
				var entry = new RTreeEntry(segment);
				Insert(entry, root);
				if (NeedsSplit(root)) {
					var newRoot = new RTreeNode(false);
					var sibling = SplitNode(root);
					newRoot.Children.Add(root);
					newRoot.Children.Add(sibling);
					newRoot.RecalculateBounds();
					root = newRoot;
				}
			}
			public void Delete(Segment target) {
				DeleteInternal(root, target);
				if (!root.IsLeaf && root.Children.Count == 1) {
					root = root.Children[0]; // flatten single-child root
				}
			}
			private bool DeleteInternal(RTreeNode node, Segment target) {
				if (node.IsLeaf) {
					for (int i = 0; i < node.Entries.Count; i++) {
						if (node.Entries[i].Segment.Equals(target)) {
							node.Entries.RemoveAt(i);
							node.RecalculateBounds();
							return true;
						}
					}
					return false;
				} else {
					foreach (var child in node.Children) {
						if (child.Bounds.Overlaps(target.GetMBR())) {
							bool deleted = DeleteInternal(child, target);
							if (deleted) {
								if (child.IsLeaf && child.Entries.Count == 0) {
									node.Children.Remove(child);
								} else if (!child.IsLeaf && child.Children.Count == 0) {
									node.Children.Remove(child);
								}
								node.RecalculateBounds();
								return true;
							}
						}
					}
					return false;
				}
			}
			private void Insert(RTreeEntry entry, RTreeNode node) {
				if (node.IsLeaf) {
					node.Entries.Add(entry);
					node.RecalculateBounds();
				} else {
					RTreeNode best = null;
					int minEnlargement = int.MaxValue;
					foreach (var child in node.Children) {
						int before = Area(child.Bounds);
						MBR expanded = MBR.Combine(child.Bounds, entry.Bounds);
						int after = Area(expanded);
						int enlarge = after - before;
						if (enlarge < minEnlargement) {
							minEnlargement = enlarge;
							best = child;
						}
					}
					Insert(entry, best);
					node.RecalculateBounds();
					if (NeedsSplit(best)) {
						var sibling = SplitNode(best);
						node.Children.Add(sibling);
					}
				}
			}
			private bool NeedsSplit(RTreeNode node) {
				return ( node.IsLeaf ) ? node.Entries.Count>RTreeNode.MaxEntries : node.Children.Count>RTreeNode.MaxEntries;
			}
			private RTreeNode SplitNode(RTreeNode node) {
				var newNode = new RTreeNode(node.IsLeaf);
				if (node.IsLeaf) {
					int mid = node.Entries.Count / 2;
					var sorted = new List<RTreeEntry>(node.Entries);
					sorted.Sort((a, b) => a.Bounds.MinX.CompareTo(b.Bounds.MinX));
					newNode.Entries.AddRange(sorted.GetRange(mid, sorted.Count - mid));
					node.Entries = sorted.GetRange(0, mid);
				} else {
					int mid = node.Children.Count / 2;
					var sorted = new List<RTreeNode>(node.Children);
					sorted.Sort((a, b) => a.Bounds.MinX.CompareTo(b.Bounds.MinX));
					newNode.Children.AddRange(sorted.GetRange(mid, sorted.Count - mid));
					node.Children = sorted.GetRange(0, mid);
				}
				node.RecalculateBounds();
				newNode.RecalculateBounds();
				return newNode;
			}
			private int Area(MBR b) {
				return (b.MaxX - b.MinX) * (b.MaxY - b.MinY);
			}
			public bool IsIntersecting(Segment query) {
				var stack = new Stack<RTreeNode>();
				stack.Push(root);
				var mbr = query.GetMBR();

				while ( stack.Count>0 ) {
					var node = stack.Pop();
					if (!node.Bounds.Overlaps(mbr)) continue;

					if (node.IsLeaf) {
						foreach (var e in node.Entries) {
							if (e.Bounds.Overlaps(mbr) && e.Segment.Intersects( query )) {
								return true;
							}
						}
					} else {
						foreach (var child in node.Children) {
							if (child.Bounds.Overlaps(mbr)) {
								stack.Push(child);
							}
						}
					}
				}
				return false;
			}
			public RTree Clone() {
				var newTree = new RTree();
				newTree.root = CloneNode(this.root);
				return newTree;
			}
			private RTreeNode CloneNode(RTreeNode node) {
				var copy = new RTreeNode(node.IsLeaf);
				if (node.IsLeaf) {
					foreach (var entry in node.Entries) {
						var seg = entry.Segment;
						copy.Entries.Add(new RTreeEntry(seg));
					}
				} else {
					foreach (var child in node.Children) {
						copy.Children.Add(CloneNode(child));
					}
				}
				copy.RecalculateBounds();
				return copy;
			}
		}

		private RTree tree = new();

		public void Insert(Segment s) => tree.Insert( s );
		public bool Intersects(Segment s) => tree.IsIntersecting(s);
		public void Delete(Segment s) => tree.Delete( s );
		public void Remove( Segment s ) => tree.Delete( s );
		public void Clear() { tree = new(); }

		public void Insert(int x0,int y0,int x1,int y1) => Insert(new Segment(x0,y0,x1,y1));
		public bool Intersects(int x0,int y0,int x1,int y1) => tree.IsIntersecting(new Segment(x0,y0,x1,y1));
		public void Remove(int x0,int y0,int x1,int y1) => Delete(new Segment(x0,y0,x1,y1));

		public SegmentRTree Duplicate() {
			var copy = new SegmentRTree();
			copy.tree = this.tree.Clone();
			return copy;
		}
	}

	//@@ **********************************************************************
	//@@ class SegmentUniformGrid
	//@@ 雑に作ったグリッドベースのこれが一番早かった。。。
	//@@ **********************************************************************
	public class SegmentUniformGrid {
		public struct Segment : IEquatable<Segment> {
 			public readonly int X0, Y0, X1, Y1;
 			public readonly int Id;
			public Segment( int x0, int y0, int x1, int y1 ) {
				X0 = x0; Y0 = y0; X1 = x1; Y1 = y1;
			}
			public Segment( int x0, int y0, int x1, int y1, int id ) : this(x0,y0,x1,y1) {
				Id = id;
			}
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			public static int Cross( int x0, int y0, int x1, int y1, int x2, int y2 ) {
				var v = ( x1 - x0 ) * ( y2 - y0 ) - ( y1 - y0 ) * ( x2 - x0 );
				return Math.Sign( v );
			}
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			public static bool Intersects( int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3 ) {
				var c0 = Cross( x0, y0, x1, y1, x2, y2 );
				var c1 = Cross( x0, y0, x1, y1, x3, y3 );
				if ( 0<c0*c1 ) return false;
				var c2 = Cross( x2, y2, x3, y3, x0, y0 );
				var c3 = Cross( x2, y2, x3, y3, x1, y1 );
				return ( c2*c3<=0 );
			}
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			public bool Intersects( Segment o ) {
				if (
					( X0==o.X0 && Y0==o.Y0 ) ||
					( X0==o.X1 && Y0==o.Y1 ) ||
					( X1==o.X0 && Y1==o.Y0 ) ||
					( X1==o.X1 && Y1==o.Y1 )
				) return false;
				return Intersects( 
					this.X0,this.Y0,
					this.X1,this.Y1,
					o.X0,o.Y0,
					o.X1,o.Y1
				);
			}
			public bool Intersects( int x2, int y2, int x3, int y3 ) {
				if (
					( X0==x2 && Y0==y2 ) ||
					( X0==x3 && Y0==y3 ) ||
					( X1==x2 && Y1==y2 ) ||
					( X1==x3 && Y1==y3 )
				) return false;
				return Intersects( 
					this.X0,this.Y0,
					this.X1,this.Y1,
					x2,y2,
					x3,y3
				);
			}
			public bool Equals( Segment o ) {
				return Id==o.Id;
//				return (X0==o.X0 && Y0==o.Y0 && X1==o.X1 && Y1==o.Y1 );
			}
		};

		private readonly int gridSize;
		private readonly int maxCoord;
		private readonly int gridNum;
		private readonly List<Segment>[] grid;
		private readonly Dictionary<int,Segment> segments;
		private readonly Dictionary<int,int> multi;
		private readonly (int,int,int,int)[] aabb;

		public SegmentUniformGrid( int gridSize=1024, int maxCoord=10000 ) {
			this.gridSize = gridSize;
			this.maxCoord = maxCoord;
			this.gridNum = ( maxCoord + gridSize - 1 ) / gridSize + 1;

			grid = new List<Segment>[gridNum*gridNum];
			for ( int i=0; i<grid.Length; i++ ) grid[i] = new List<Segment>( 64 );
			segments = new Dictionary<int, Segment>();
			multi = new Dictionary<int, int>();
			aabb = new (int,int,int,int)[gridNum*gridNum];
			for ( int yi=0; yi<gridNum; yi++ ) {
				for ( int xi=0; xi<gridNum; xi++ ) {
					aabb[yi*gridNum+xi] = (xi*gridSize-1,(xi+1)*gridSize,yi*gridSize-1,(yi+1)*gridSize);
				}
			}
		}

		public void Clear() {
			for ( int i=0; i<grid.Length; i++ ) grid[i].Clear();
			segments.Clear();
			multi.Clear();
		}

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public void Add( int X0, int Y0, int X1, int Y1, int id  ) {
			if ( multi.ContainsKey(id) ) { 
				if ( 1<multi[id] ) {
					std.Throw( $"SegmentUniformGrid.Add > multi(id:{id})={multi[id]} {id} {(id)>>16} -> {id&0xffff} is fatal error" );
				}
				// 2本目の追加はハッシュで済ます
				multi[id]++;
				return;
			}

			if (segments.ContainsKey(id)) {
				std.Throw( $"SegmentUniformGrid.Add > segments.ContainsKey(id:{id}) is fatal error" );
			}
			var seg = new Segment(X0,Y0,X1,Y1,id);
			segments[id] = seg;

			int a0 = X1 - X0;
			int b0 = Y1 - Y0;
			int c0 = Y0 * a0 - X0 * b0;

//			int minX0 = Math.Min(X0,X1);
//			int maxX0 = Math.Max(X0,X1);
//			int minY0 = Math.Min(Y0,Y1);
//			int maxY0 = Math.Max(Y0,Y1);
			int x0 = X0 / gridSize;
			int y0 = Y0 / gridSize;
			int x1 = X1 / gridSize;
			int y1 = Y1 / gridSize;
			int minx = Math.Min(x0,x1);
			int maxx = Math.Max(x0,x1);
			int miny = Math.Min(y0,y1);
			int maxy = Math.Max(y0,y1);
			for ( int yi=miny; yi<=maxy; yi++ ) {
				var found = false;
				for ( int xi=minx; xi<=maxx; xi++ ) {
					var p = yi*gridNum+xi;
					var (minX1,maxX1,minY1,maxY1) = aabb[p];
//					if ( maxX1<minX0 || minX1>maxX0 || maxY1<minY0 || minY1>maxY0 ) continue;

					// グリッドと線分の衝突判定（線分を直線扱いにして計算削減
					var s0 = Math.Sign( minX1 * b0 - minY1 * a0 + c0 );
					var s1 = Math.Sign( minX1 * b0 - maxY1 * a0 + c0 );
					if ( s0!=s1 ) goto END;
					var s2 = Math.Sign( maxX1 * b0 - minY1 * a0 + c0 );
					if ( s0!=s2 ) goto END;
					var s3 = Math.Sign( maxX1 * b0 - maxY1 * a0 + c0 );
					if ( s0!=s3 ) goto END;
					if ( found ) break;
					continue;
					END:

					grid[p].Add(seg);
					found = true;
				}
			}
			multi[id] = 1;
		}

		public int Remove( int id ) {
			var cnt = 0;
			if ( multi.ContainsKey(id) ) {
			if ( !multi.TryGetValue(id, out cnt) ) {
				std.Throw( $"SegmentUniformGrid.Remove > multi(id:{id})={cnt} is multi error" );
			}
			}

			if ( cnt==0 ) {
				std.Throw( $"SegmentUniformGrid.Remove > multi(id:{id})={cnt} is fatal error" );
			} else if ( cnt==1 ) {
				multi[id] = 0;
			} else if ( 1<cnt ) {
				// 2本目の削除はハッシュで済ます
				multi[id] = cnt - 1;
				if ( !segments.TryGetValue(id, out var seg0 ) ) {
				}
				return cnt - 1;
			} else {
				std.Throw( $"SegmentUniformGrid.Remove > multi(id:{id})={cnt} is fatal error" );
			}

			if ( !segments.TryGetValue(id, out var seg ) ) {
				std.Throw( $"SegmentUniformGrid.Remove > multi(id:{id})={cnt}, segments is fatal error" );
			}

			int a0 = seg.X1 - seg.X0;
			int b0 = seg.Y1 - seg.Y0;
			int c0 = seg.Y0 * a0 - seg.X0 * b0;
//			int minX0 = Math.Min(seg.X0,seg.X1);
//			int maxX0 = Math.Max(seg.X0,seg.X1);
//			int minY0 = Math.Min(seg.Y0,seg.Y1);
//			int maxY0 = Math.Max(seg.Y0,seg.Y1);
			int x0 = seg.X0 / gridSize;
			int y0 = seg.Y0 / gridSize;
			int x1 = seg.X1 / gridSize;
			int y1 = seg.Y1 / gridSize;
			int minx = Math.Min(x0,x1);
			int maxx = Math.Max(x0,x1);
			int miny = Math.Min(y0,y1);
			int maxy = Math.Max(y0,y1);
			var removeCnt = 0;
			for ( int xi=minx; xi<=maxx; xi++ ) {
				var found = false;
				for ( int yi=miny; yi<=maxy; yi++ ) {
					var p = yi*gridNum+xi;
					var (minX1,maxX1,minY1,maxY1) = aabb[p];
//					if ( maxX1<minX0 || minX1>maxX0 || maxY1<minY0 || minY1>maxY0 ) continue;

					// グリッドと線分の衝突判定（線分を直線扱いにして計算削減
					var s0 = Math.Sign( minX1 * b0 - minY1 * a0 + c0 );
					var s1 = Math.Sign( minX1 * b0 - maxY1 * a0 + c0 );
					if ( s0!=s1 ) goto END;
					var s2 = Math.Sign( maxX1 * b0 - minY1 * a0 + c0 );
					if ( s0!=s2 ) goto END;
					var s3 = Math.Sign( maxX1 * b0 - maxY1 * a0 + c0 );
					if ( s0!=s3 ) goto END;
					if ( found ) break;
					continue;

					END:

					removeCnt += grid[p].RemoveAll(s=>s.Id==id);
					found = true;
				}
			}
			segments.Remove(id);
			multi.Remove(id);
			return removeCnt;
		}

		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public bool Intersects( int X0, int Y0, int X1, int Y1, int id ) {

			int a0 = X1 - X0;
			int b0 = Y1 - Y0;
			int c0 = Y0 * a0 - X0 * b0;
//			int minX0 = Math.Min(X0,X1);
//			int maxX0 = Math.Max(X0,X1);
//			int minY0 = Math.Min(Y0,Y1);
//			int maxY0 = Math.Max(Y0,Y1);
			int x0 = X0 / gridSize;
			int y0 = Y0 / gridSize;
			int x1 = X1 / gridSize;
			int y1 = Y1 / gridSize;
			int minx = Math.Min(x0,x1);
			int maxx = Math.Max(x0,x1);
			int miny = Math.Min(y0,y1);
			int maxy = Math.Max(y0,y1);
			for ( int xi=minx; xi<=maxx; xi++ ) {
				var found = false;
				for ( int yi=miny; yi<=maxy; yi++ ) {
					var p = yi*gridNum+xi;
					var (minX1,maxX1,minY1,maxY1) = aabb[p];
//					if ( maxX1<minX0 || minX1>maxX0 || maxY1<minY0 || minY1>maxY0 ) continue;

					// グリッドと線分の衝突判定（線分を直線扱いにして計算削減
					var s0 = Math.Sign( minX1 * b0 - minY1 * a0 + c0 );
					var s1 = Math.Sign( minX1 * b0 - maxY1 * a0 + c0 );
					if ( s0!=s1 ) goto END;
					var s2 = Math.Sign( maxX1 * b0 - minY1 * a0 + c0 );
					if ( s0!=s2 ) goto END;
					var s3 = Math.Sign( maxX1 * b0 - maxY1 * a0 + c0 );
					if ( s0!=s3 ) goto END;
					if ( found ) break;
					continue;
					END:

					var gd = grid[p];
					for ( int i=0; i<gd.Count; i++ ) {
						if (gd[i].Intersects(X0,Y0,X1,Y1)) {
							return true;
						}
					}
					found = true;
				}
			}
			return false;
		}
		public void CopyFrom( SegmentUniformGrid o ) {
			Clear();
			foreach (var seg in o.segments.Values) {
				var cnt = o.multi[seg.Id];
				for ( int i=0; i<cnt; i++ ) {
					Add(seg.X0,seg.Y0,seg.X1,seg.Y1,seg.Id);
				}
			}
		}
		public SegmentUniformGrid Duplicate() {
			var o = new SegmentUniformGrid(gridSize, maxCoord);
			foreach (var seg in segments.Values) {
				var cnt = multi[seg.Id];
				for ( int i=0; i<cnt; i++ ) {
					o.Add(seg.X0,seg.Y0,seg.X1,seg.Y1,seg.Id);
				}
			}
			return o;
		}
	}

	//@@ **********************************************************************
	//@@ class Input
	//@@ **********************************************************************
	public class Input {

		//@@ field -------------------------------------------------------
		public static RandomX rand = new RandomX( 42 );
		public int N; // 5 <= N <= 20
		public int M; // 10N <= M <= 50N
		public int K; //  N <= K <=  4N
		public int NM;// N+M
		public int[][] XY = null;
		public double[][] KP = null;

		//@@ ctor -------------------------------------------------------
		static Input() {
		}
		public void CopyFrom( Input o ) {
			(this.N,this.M,this.K) = (o.N,o.M,o.K);
			this.NM = o.NM;
			this.XY.CopyFrom( o.XY );
			this.KP.CopyFrom( o.KP );
		}
		public Input Dup() {
			var o = new Input();
			o.CopyFrom( this );
			return o;
		}

		//@@ method -------------------------------------------------------
		public static Input LoadStd() {
			var o = new Input();
			(o.N,o.M,o.K) = std.ReadIX();
			o.NM = o.N + o.M;
			o.XY = std.ReadIXY( o.NM );
			o.KP = std.ReadDXY( o.K );
			return o;
		}
		public static Input LoadFile( string path ) {
			var o = new Input();
			var lines = File.ReadAllLines( path );
			var p = 0;
			(o.N,o.M,o.K) = lines[p++].Split().Select( i => Int32.Parse( i ) ).ToArray();
			o.NM = o.N + o.M;

			o.XY = std.Arr2<int>( o.NM );
			for ( int i=0; i<o.K; i++ ) {
				o.XY[i] = lines[p++].Split().Select( i => Int32.Parse( i ) ).ToArray();
			}
			o.KP = std.Arr2<double>( o.K );
			for ( int i=0; i<o.K; i++ ) {
				o.KP[i] = lines[p++].Split().Select( i => Double.Parse( i ) ).ToArray();
			}
			return o;
		}
		public static Input LoadGen() {
			var N = rand.NextInt( 5, 20 );
			var M = rand.NextInt( 10*N, 50*N );
			var K = rand.NextInt( N, 4*N );
			return LoadGen( N, M, K );
		}
		public static Input LoadGen( int N, int M, int K ) {
			var NM = N+M;
			var xy = std.Arr2<int>( NM + 1, 2 );
			var cnt = 1;
			xy[0][0] = 0;
			xy[0][1] = 5000;
			while ( cnt<=NM ) {
				var (x,y) = (rand.NextInt(10000),rand.NextInt(10000));
				var inc = 1;
				for ( int i=0; i<cnt; i++ ) {
					var (dx,dy) = (x-xy[i][0],y-xy[i][1]);
					var d = dx*dx+dy*dy;
					if ( d<=100*100 ) {
						inc = 0;
						break;
					}
				}
				xy[cnt][0] = x;
				xy[cnt][1] = y;
				cnt += inc;
			}
			var kp = std.Arr2<double>( K, N );
			for ( int i=0; i<K; i++ ) {
				for ( int j=0; j<N; j++ ) {
					kp[i][j] = ( 1000 + rand.NextInt(9000) ) * 1.0e-4;
				}
			}
			var o = new Input();
			(o.N,o.M,o.K,o.NM) = (N,M,K,NM);
			o.XY = std.Arr2<int>( NM, 2 );
			for ( int i=0; i<NM; i++ ) {
				o.XY[i][0] = xy[i+1][0];
				o.XY[i][1] = xy[i+1][1];
			}
			o.KP = kp;
			return o;
		}
	}

	//@@ **********************************************************************
	//@@ class Terminal
	//@@ **********************************************************************
	public class Terminal {

		//@@ field -------------------------------------------------------
		public static RandomX rand = new RandomX( 42 );
		public static int N;
		public static int M;
		public static int K;
		public static int NM;
		public static int EstScore;

		public static int[][] Xys = null;
		public static List<int[]> ConvexXys = null;
		public static List<int[]> ConvexLXys = null;
		public static List<int[]> ConvexRXys = null;
		public static List<int[]> HorizenXys = null;
		public static double[][] Probs0 = null;
		public static double[][] Probs1 = null;

		public const int MAX_NEAR = 128;
		public static int[][] Nears0 = null;
		public static int[][] Nears1 = null;
		public static int[][] NearsA = null;
//		public static int[][] NearsAy = null;
//		public static int[][] NearsAx = null;

		public static void PreInitialize( Input input ) {
			N = input.N;
			M = input.M;
			K = input.K;
			NM = input.NM;

			Xys = std.Arr2<int>( NM+1, 2 );
			for ( int i=0; i<NM; i++ ) {
				Xys[i][0] = input.XY[i][0];
				Xys[i][1] = input.XY[i][1];
			}
			Xys[NM][0] = 0;// 搬入口X
			Xys[NM][1] = 5000;// 搬入口Y
			Probs0 = input.KP.Dup();
			Probs1 = std.Arr2<double>( K, N );
			for ( int i=0; i<K; i++ ) {
				for ( int j=0; j<N; j++ ) {
					Probs1[i][j] = 1.0 - Probs0[i][j];
				}
			}

			var dists = std.Arr2<(int,int)>( NM+1, NM+1, (i,j)=>(int.MaxValue,j) );
			var distsx = std.Arr2<(int,int)>( NM+1, NM+1, (i,j)=>(int.MaxValue,j) );
			var distsy = std.Arr2<(int,int)>( NM+1, NM+1, (i,j)=>(int.MaxValue,j) );
			var useSize = Math.Min( NM, N+Prm.prmUseSorterNum );
			var maxNear = Math.Min( NM, MAX_NEAR );
			for ( int i=0; i<useSize; i++ ) {
				var (x0,y0) = Xys[i];
				for ( int j=i+1; j<useSize; j++ ) {
					var (x1,y1) = Xys[j];
					var xx = (x0-x1)*(x0-x1);
					var yy = (y0-y1)*(y0-y1);
					var d = xx+yy;
					var dx = Prm.prmDistXZoomRatio*xx+yy;
					var dy = Prm.prmDistXZoomRatio*yy+xx;
					dists[i][j] = ((int)(d/10000),j);
					dists[j][i] = ((int)(d/10000),i);
					distsx[i][j] = ((int)(dx/10000),j);
					distsx[j][i] = ((int)(dx/10000),i);
					distsy[i][j] = ((int)(dy/10000),j);
					distsy[j][i] = ((int)(dy/10000),i);
				}
			}
			for ( int i=0; i<useSize; i++ ) {
				var (x0,y0) = Xys[NM];
				var (x1,y1) = Xys[i];
				var xx = (x0-x1)*(x0-x1);
				var yy = (y0-y1)*(y0-y1);
				var d = xx+yy;
				var dx = Prm.prmDistXZoomRatio*xx+yy;
				var dy = Prm.prmDistXZoomRatio*yy+xx;
				dists[NM][i] = ((int)(d/10000),i);
				distsx[NM][i] = ((int)(dx/10000),i);
				distsy[NM][i] = ((int)(dy/10000),i);
			}

			Nears0 = std.Arr2<int>( NM+1, maxNear );
			Nears1 = std.Arr2<int>( NM+1, maxNear );
			NearsA = std.Arr2<int>( NM+1, maxNear );
//			NearsAx = std.Arr2<int>( NM+1, maxNear );
//			NearsAy = std.Arr2<int>( NM+1, maxNear );
			for ( int i=0; i<NM+1; i++ ) {
				var ds = dists[i];
				Sort.RadixSort( ds.AsSpan(), i=>i.Item1 );
				var j = 0;
				var p = 0;
				while ( p<maxNear && j<NM ) {
					var u = ds[j++].Item2;
					if ( u<N ) continue;
					Nears0[i][p] = u;
					p++;
				}
				j = 0;
				p = 0;
				while ( p<maxNear && j<NM ) {
					var u = ds[j++].Item2;
					if ( N<=u ) continue;
					Nears1[i][p] = u;
					p++;
				}
				p = 0;
				while ( p<maxNear ) {
					var u = ds[p].Item2;
					NearsA[i][p] = u;
					p++;
				}
//				ds = distsx[i];
//				Sort.RadixSort( ds.AsSpan(), i=>i.Item1 );
//				p = 0;
//				while ( p<maxNear ) {
//					var u = ds[p].Item2;
//					NearsAx[i][p] = u;
//					p++;
//				}
//				ds = distsy[i];
//				Sort.RadixSort( ds.AsSpan(), i=>i.Item1 );
//				p = 0;
//				while ( p<maxNear ) {
//					var u = ds[p].Item2;
//					NearsAy[i][p] = u;
//					p++;
//				}
			}

			// (0,5000)が先頭の凸包を作る
			var buf = Util.ConvexHull( Terminal.Xys, NM+1 );
			ConvexXys = new List<int[]>();
			for ( int i=0; i<buf.Count; i++ ) {
				if ( buf[i][0]==0 && buf[i][1]==5000 ) {
					for ( int j=i; j<buf.Count; j++ ) ConvexXys.Add( buf[j] );
					for ( int j=0; j<i; j++ ) ConvexXys.Add( buf[j] );
					break;
				}
			}

			// 搬入口から凸包右回りと左回りを作る
			ConvexRXys = new List<int[]>();
			ConvexLXys = new List<int[]>();
			for ( int i=0; i<ConvexXys.Count; i++ ) {
				if ( ConvexXys[i][2]<N ) break;
				ConvexRXys.Add( ConvexXys[i] );
			}
			ConvexLXys.Add( ConvexXys[0] );
			for ( int i=ConvexXys.Count-1; 0<i; i-- ) {
				if ( ConvexXys[i][2]<N ) break;
				ConvexLXys.Add( ConvexXys[i] );
			}

			HorizenXys = new List<int[]>();
			HorizenXys.Add( new int[3]{0,5000,NM} );
			var dst = (10000,5000);
			Util.visId++;
			for ( int i=0; i<32; i++ ) {
				var u = HorizenXys[HorizenXys.Count-1][2];
				var nears = NearsA[u];
				var minIdx = 0;
				var minDis = double.MaxValue;
				for ( int j=0; j<32; j++ ) {
					var v = nears[j];
					if ( v<N || Util.vis[v]==Util.visId ) continue;
					var (x,y) = Xys[nears[j]];
					var dx = (x-dst.Item1);
					var dy = (y-dst.Item2);
					var dd = dx*dx+dy*dy;
					if ( dd<minDis ) {
						minDis = dd;
						minIdx = j;
					}
				}
				var u0 = nears[minIdx];
				var xy0 = Xys[u0];
				HorizenXys.Add( new int []{xy0[0],xy0[1],u0} );
				Util.vis[u0] = Util.visId;
				if ( 5000<xy0[0] ) break;
			}

//			std.sep();
//			foreach ( var p in HorizenXys ) {
//				std.Print(p.Str());
//			}

//			std.sep();
//			foreach ( var p in ConvexXys ) {
//				std.Print(p.Str());
//			}
//			std.sep();
//			foreach ( var p in ConvexRXys ) {
//				std.Print(p.Str());
//			}
//			std.sep();
//			foreach ( var p in ConvexLXys ) {
//				std.Print(p.Str());
//			}

			EstScore = (int)Prm.EstScore( N, M, K );
			if ( Prm.isSim || Prm.isLoad ) {
			}
		}

		//@@ ctor -------------------------------------------------------
		static Terminal() {
		}
		//@@ method -------------------------------------------------------
		public static int Dist2( int x0, int y0, int x1, int y1 ) {
			return (x0-x1)*(x0-x1)+(y0-y1)*(y0-y1);
		}

		//@@ Print -------------------------------------------------------
		public static void Print( bool isDetail=false ) {
			std.Print($"Terminal.N.M.K = {N} {M} {K}");
		}
	}

	//@@ **********************************************************************
	//@@ class Util
	//@@ **********************************************************************
	public class Util {
		//@@ field -------------------------------------------------------
		public const int MAX_QUE_SIZE = 1024;
		public static int visId = 0;
		public static int[] vis = new int[MAX_QUE_SIZE];
		public static QueuePow2<int> que = new QueuePow2<int>( MAX_QUE_SIZE ); // N+Mの最大値は1020

		//@@ method -------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static int Cross( int x0, int y0, int x1, int y1, int x2, int y2 ) {
			var v = ( x1 - x0 ) * ( y2 - y0 ) - ( y1 - y0 ) * ( x2 - x0 );
			return Math.Sign( v );
		}
		public static bool Intersects( int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3 ) {
			if (
				Math.Max(x0,x1)<Math.Min(x2,x3) ||
				Math.Max(x2,x3)<Math.Min(x0,x1) ||
				Math.Max(y0,y1)<Math.Min(y2,y3) ||
				Math.Max(y2,y3)<Math.Min(y0,y1)
			) {
				return false;
			}
			var c0 = Cross( x0, y0, x1, y1, x2, y2 );
			var c1 = Cross( x0, y0, x1, y1, x3, y3 );
			if ( 0<c0*c1 ) return false;
			var c2 = Cross( x2, y2, x3, y3, x0, y0 );
			var c3 = Cross( x2, y2, x3, y3, x1, y1 );
			return ( c2*c3<=0 );
		}
		public static bool IntersectsBrute( List<(int,int,int,int)> es, int u, int v ) {
			var xys = Terminal.Xys;
			var (x0,y0) = xys[u];
			var (x1,y1) = xys[v];
			for ( int i=0; i<es.Count; i++ ) {
				var (x2,y2,x3,y3) = es[i];
				if (
					( x0==x2 && y0==y2 ) ||
					( x0==x3 && y0==y3 ) ||
					( x1==x2 && y1==y2 ) ||
					( x1==x3 && y1==y3 )
				) continue;
				if ( Util.Intersects( x0,y0,x1,y1,x2,y2,x3,y3 ) ) {
					return true;
				}
			}
			return false;
		}
		public static bool IntersectsQt( SegmentQuadTree qt, int u, int v ) {
			var xys = Terminal.Xys;
			var (x0,y0) = xys[u];
			var (x1,y1) = xys[v];
			return qt.Intersects( x0,y0,x1,y1 );
		}
		public static bool IntersectsRt( SegmentRTree rt, int u, int v ) {
			var xys = Terminal.Xys;
			var (x0,y0) = xys[u];
			var (x1,y1) = xys[v];
			return rt.Intersects( x0,y0,x1,y1 );
		}
		public static bool IntersectsUg( SegmentUniformGrid ug, int u, int v ) {
			var xys = Terminal.Xys;
			var (x0,y0) = xys[u];
			var (x1,y1) = xys[v];
			return ug.Intersects( x0,y0,x1,y1,(u<<16)+v );
		}
		public static bool IsCycle( (int,int)[] gs, int u0, int s0 ) {
			visId++;
			que.Clear();
			que.Push( s0 );
			vis[s0] = visId;
			while ( que.Exists ) {
				var u = que.Pop();
				var (v0,v1) = gs[u];
				if ( v0!=-1 && vis[v0]!=visId ) {
					if ( v0==u0 ) return true;
					vis[v0] = visId;
					que.Push( v0 );
				}
				if ( v1!=-1 && vis[v1]!=visId ) {
					if ( v1==u0 ) return true;
					vis[v1] = visId;
					que.Push( v1 );
				}
			}
			return false;
		}

		public static List<int[]> ConvexHull( int[][] xys, int size ) {
			var xys0 = new int[size][];
			for ( int i=0; i<size; i++ ) {
				var xy = new int[3];
				xy[0] = xys[i][0];
				xy[1] = xys[i][1];
				xy[2] = i;
				xys0[i] = xy;
			}
			Array.Sort( xys0, (a,b)=>(a[0]!=b[0])?((a[0]<b[0])?-1:1):((a[1]<b[1])?-1:1) );
			var lower = new List<int[]>();
			foreach ( var xy in xys0 ) {
				while ( lower.Count>=2 && Cross( lower[^2][0], lower[^2][1], lower[^1][0], lower[^1][1], xy[0], xy[1] )<= 0 ) {
					lower.RemoveAt( lower.Count-1 );
				}
				lower.Add( xy );
			}
			var upper = new List<int[]>();
			for ( int i=size-1; i>=0; i-- ) {
				var xy = xys0[i];
				while ( upper.Count>=2 && Cross( upper[^2][0], upper[^2][1], upper[^1][0], upper[^1][1], xy[0], xy[1] )<=0 ) {
					upper.RemoveAt( upper.Count-1 );
				}
				upper.Add( xy );
			}
			lower.RemoveAt( lower.Count-1 );
			upper.RemoveAt( upper.Count-1 );
			lower.AddRange( upper );
			return lower;
		}
//		public static int[][] GetNearsA( int zoom ) {
//			return ( zoom==0 ) ? Terminal.NearsA : ( zoom==1 ) ? Terminal.NearsAx : Terminal.NearsAy;
//		}
	}

	//@@ **********************************************************************
	//@@ class Transition
	//@@ **********************************************************************
	public class Transition {
		//@@ static -------------------------------------------------------
//		public static ObjectPoolCls<Transition> pool = new ObjectPoolCls<Transition>(()=>new Transition(),initialCapacity:1024*1024);
		public static DisposableObjectPoolCls<Transition> pool = new DisposableObjectPoolCls<Transition>(()=>new Transition(),isPreInit:true,initialCapacity:2*1024);

		//@@ field -------------------------------------------------------
		public double score;

		// 遷移情報
		public int type;
		public int target0;
		public int sub0;
		public int sub1;
		public int sub2;
		public int sorterIdx0;
		public int swap;
		public int single;

		//@@ ctor -------------------------------------------------------
		public Transition() {
		}
		//@@ method -------------------------------------------------------
	}

	//@@ **********************************************************************
	//@@ class Sample
	//@@ **********************************************************************
	public class Sample {
		//@@ static -------------------------------------------------------
		public static RandomX rand = new RandomX( 42 );
		public static ObjectPoolCls<Sample> pool = new ObjectPoolCls<Sample>(()=>new Sample(),isPreInit:false,isPreInitSize:32*1024,initialCapacity:64*1024);

		public const float EPS = 1.0e-6f;
		public static int N;
		public static int M;
		public static int K;
		public static int NM;

		public static int visId = 0;
		public static int[] vis = new int[1024];
		public static QueuePow2<int> que = new QueuePow2<int>( 1024 ); // N+Mの最大値は1020

		//@@ field -------------------------------------------------------
		public double score;

		// 内部状態
		public int seed;
		public int root;
//		public int zoom;
		public double temperature;
		public List<(int,int,int,int)> es = null;
//		public SegmentQuadTree qt = new SegmentQuadTree(0,10000,0,10000);
//		public SegmentRTree rt = new SegmentRTree();
		public SegmentUniformGrid ug = new SegmentUniformGrid( gridSize:Prm.prmUniformGridSize, maxCoord:10000 );
		public (int,int)[] gs = null;
		public List<int>[] rs = null;
		public int[] assigns = null;
		public double[][] probs = null;
		public double[][] drains = null;
//		public double[][] deltas = null;
//		public double[] errors = null;
		public int[] invalidProbs = null;
		public int[] invalidDrains = null;
		public List<int> orders = null;
		public int shuffleIdx;
		public int[] shuffleIdxs = null;

		public double[] cachet0v0 = null;
		public double[] cachet0v1 = null;
		public double[] cachet1v0 = null;
		public double[] cachet1v1 = null;
		public double[] cachet0vx = null;
		public double[] cachet1vx = null;

		//@@ ctor -------------------------------------------------------
		static Sample() {
		}
		public static void PreInitialize() {
			N = Terminal.N;
			M = Terminal.M;
			K = Terminal.K;
			NM = Terminal.NM;
		}
		public Sample() {
			score = Int32.MaxValue;
			seed = -1;
			root = -1;
//			zoom = 0;
			temperature = 0.0;
			es = new List<(int,int,int,int)>( NM );
			gs = std.Arr1<(int,int)>( NM, (-1,-1) );
			rs = std.Arr1<List<int>>( NM, i=> new List<int>( 8 ) );
			assigns = std.Arr1<int>( NM, -1 );

			probs = std.Arr2<double>( NM, N );
			drains = std.Arr2<double>( NM, N );
//			deltas = std.Arr2<double>( NM, N );
//			errors = std.Arr1<double>( NM );
			invalidProbs = std.Arr1<int>( NM, 1 );
			invalidDrains = std.Arr1<int>( NM, 1 );
			orders = new List<int>();
			shuffleIdx = NM;
			shuffleIdxs = std.Arr1<int>( M, i=>i+N );

			cachet0v0 = new double[K];
			cachet0v1 = new double[K];
			cachet1v0 = new double[K];
			cachet1v1 = new double[K];
			cachet0vx = new double[K];
			cachet1vx = new double[K];
		}
		public Sample( Sample o ) {
			this.CopyFrom( o );
		}
		public Sample Dup() {
			var o = pool.Alloc();
			o.CopyFrom( this );
			return o;
		}
		public void CopyFrom( Sample o ) {
			if ( this==o ) return;
			this.score = o.score;
			this.seed = o.seed;
			this.root = o.root;
//			this.zoom = o.zoom;
			this.temperature = o.temperature;

			this.gs.CopyFrom( o.gs );
			for ( int i=0; i<NM; i++ ) {
				this.rs[i].Clear();
				this.rs[i].AddRange( o.rs[i] );
			}

			this.assigns.CopyFrom( o.assigns );
			this.probs.CopyFrom( o.probs );
			this.drains.CopyFrom( o.drains );
//			this.deltas.CopyFrom( o.deltas );
//			this.errors.CopyFrom( o.errors );
			this.invalidProbs.CopyFrom( o.invalidProbs );
			this.invalidDrains.CopyFrom( o.invalidDrains );
			this.orders.Clear(); this.orders.AddRange( o.orders );

//			if ( Prm.prmIntersectsQuadTree ) {
//				this.qt.CopyFrom( o.qt );
//			} else if ( Prm.prmIntersectsRTree ) {
//				this.rt = o.rt.Duplicate();
//			} else if ( Prm.prmIntersectsUniformGrid ) {
				this.ug.CopyFrom( o.ug );
//			} else {
//				this.es.Clear(); this.es.AddRange( o.es );
//			}
		}

		public void CopyFromForSA( Sample o ) {
			if ( this==o ) return;
			this.score = o.score;
			this.root = o.root;
//			this.zoom = o.zoom;
			this.temperature = o.temperature;
			this.gs.CopyFrom( o.gs );
			this.assigns.CopyFrom( o.assigns );
		}

		//@@ Util -------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public bool Intersects( int u, int v ) {
			var isIntersects = false;
//			if ( Prm.prmIntersectsQuadTree ) {
//				isIntersects = Util.IntersectsQt( qt, u, v );
//			} else if ( Prm.prmIntersectsRTree ) {
//				isIntersects = Util.IntersectsRt( rt, u, v );
//			} else if ( Prm.prmIntersectsUniformGrid ) {
				isIntersects = Util.IntersectsUg( ug, u, v );
//			} else {
//				isIntersects = Util.IntersectsBrute( es, u, v );
//			}
			return isIntersects;
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public void PlaceBeltWithoutCheck( int u, int v0, int v1 ) {
			gs[u] = (v0,v1);
			rs[v0].Add( u );
			rs[v1].Add( u );
			var xys = Terminal.Xys;
			AddEs(xys[u][0],xys[u][1],xys[v0][0],xys[v0][1],u,v0);
			AddEs(xys[u][0],xys[u][1],xys[v1][0],xys[v1][1],u,v1);
		}
		public int PopShuffleSorterIdx() {
			shuffleIdx++;
			if ( shuffleIdxs.Length<=shuffleIdx ) {
				shuffleIdx = 0;
				shuffleIdxs.Shuffle0();
			}
			return shuffleIdxs[shuffleIdx];
		}
//		public int PopShuffleSorterIdx2( int tryNum ) {
//			var maxIdx = 0;
//			var maxVal = 0.0;
//			for ( int i=0; i<tryNum; i++ ) {
//				var u = PopShuffleSorterIdx();
//				if ( u<N ) return u;
//				if ( maxVal<errors[u] ) {
//					maxVal = errors[u];
//					maxIdx = u;
//				}
//			}
//			return maxIdx;
//		}

		//@@ AddEs -------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public void AddEs( int x0, int y0, int x1, int y1, int u, int v ) {
//			if ( Prm.prmIntersectsQuadTree ) {
//				qt.Add((x0,y0,x1,y1));
//			} else if ( Prm.prmIntersectsRTree ) {
//				rt.Insert(x0,y0,x1,y1);
//			} else if ( Prm.prmIntersectsUniformGrid ) {
				ug.Add(x0,y0,x1,y1,(u<<16)+v);
//			} else {
//				es.Add((x0,y0,x1,y1));
//			}
		}
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public void AddEs( int u, int v ) {
			var xys = Terminal.Xys;
			AddEs(xys[u][0],xys[u][1],xys[v][0],xys[v][1],u,v);
		}

		//@@ RemoveEs -------------------------------------------------------
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public int RemoveEs( int u, int v ) {
//			if ( Prm.prmIntersectsQuadTree ) {
//				var xys = Terminal.Xys;
//				qt.Remove(xys[u][0],xys[u][1],xys[v][0],xys[v][1]);
//			} else if ( Prm.prmIntersectsRTree ) {
//				var xys = Terminal.Xys;
//				rt.Remove(xys[u][0],xys[u][1],xys[v][0],xys[v][1]);
//			} else if ( Prm.prmIntersectsUniformGrid ) {
				var xys = Terminal.Xys;
				return ug.Remove((u<<16)+v);
//			} else {
//			}
//			return -1;
		}

		//@@ Clear -------------------------------------------------------
		public void Clear() {
			for ( int i=0; i<NM; i++ ) rs[i].Clear();
			Array.Fill( gs, (-1,-1) );
			Array.Fill( assigns, -1 );
			es.Clear();
//			qt.Clear();
//			rt.Clear();
			ug.Clear();
		}
		public void ClearEs() {
			es.Clear();
//			qt.Clear();
//			rt.Clear();
			ug.Clear();
		}
		public void ClearRs() {
			for ( int i=0; i<NM; i++ ) rs[i].Clear();
		}
		public void ClearGs() {
			Array.Fill( gs, (-1,-1) );
		}
		public void ClearAssigns() {
			Array.Fill( assigns, -1 );
		}

		//@@ Remove -------------------------------------------------------
		public void RemoveSorter( int u ) {
			if ( u<N ) return;
			var (v0,v1) = gs[u];
			if ( v0==v1 ) {
				if ( v0!=-1 && rs[v0].Count==2 ) RemoveSorter( v0 );
			} else {
				if ( v0!=-1 && rs[v0].Count==1 ) RemoveSorter( v0 );
				if ( v1!=-1 && rs[v1].Count==1 ) RemoveSorter( v1 );
			}
			gs[u] = (-1,-1);
			rs[u].Clear();
			assigns[u] = -1;
		}
		public void RemoveInvalidSorter() {
			que.Clear();
			for ( int u=N; u<NM; u++ ) {
				var (v0,v1) = gs[u];
				if ( v0==-1 || v1==-1 ) {
					que.Push( u );
				}
			}
			while ( que.Exists ) {
				var u = que.Pop();
				for ( int i=0; i<rs[u].Count; i++ ) {
					var v = rs[u][i];
					if ( v==-1 ) continue; // 搬入口はスキップ
					var (v0,v1) = gs[v];
					if ( v0==u ) v0 = -1;//自己ループ削除
					if ( v1==u ) v1 = -1;//自己ループ削除
					if ( v0==-1 && v1==-1 ) {
						que.Push( v );
					} else if ( v0==-1 ) {
						v0 = v1;
					} else if ( v1==-1 ) {
						v1 = v0;
					}
					gs[v] = (v0,v1);
				}
				assigns[u] = -1;
				rs[u].Clear();
			}
		}
		public void RemoveNoUseSorter() {
			que.Clear();
			for ( int u=N; u<NM; u++ ) {
				if ( rs[u].Count==0 ) {
					que.Push( u );
				}
			}
			while ( que.Exists ) {
				var u = que.Pop();
				var (v0,v1) = gs[u];
				if ( v0!=-1 ) {
					rs[v0].Remove( u );
					if ( rs[v0].Count==0 && u<N ) que.Push( v0 );
				}
				if ( v1!=-1 ) {
					rs[v1].Remove( u );
					if ( rs[v1].Count==0 && u<N ) que.Push( v1 );
				}
				assigns[u] = -1;
			}
		}

		//@@ Rebuild -------------------------------------------------------
		public void RebuildRootLeafProbDrain() {
			Array.Fill( probs[root], 0.0 );
			for ( int i=0; i<N; i++ ) {
				probs[root][i] = 1.0;
			}
			for ( int i=0; i<N; i++ ) {
				Array.Fill( drains[i], 0.0 );
//				Array.Fill( deltas[i], 0.0 );
				drains[i][assigns[i]] = 1.0;
			}
		}
		public void RebuildOrder() {
			var deg = new int[NM];
			for ( int i=0; i<N; i++ ) deg[i] = 0;
			for ( int i=N; i<NM; i++ ) deg[i] = 2;

			que.Clear();
			for ( int i=0; i<N; i++ ) que.Push( i );
			orders.Clear();
			while ( que.Exists ) {
				var u = que.Pop();
				if ( assigns[u]!=-1 ) orders.Add( u );
				for ( int i=0; i<rs[u].Count; i++ )  {
					var v = rs[u][i];
					if ( v==-1 ) continue; // 搬入口は -1
					deg[v]--;
					if ( deg[v]==0 ) que.Push( v );
				}
			}
		}
		public void RebuildEs() {
//			if ( Prm.prmIntersectsQuadTree ) {
//				this.qt.Clear();
//			} else if ( Prm.prmIntersectsRTree ) {
//				this.rt.Clear();
//			} else if ( Prm.prmIntersectsUniformGrid ) {
				this.ug.Clear();
//			} else {
//				this.es.Clear();
//			}
			var xys = Terminal.Xys;
			var (x0,y0) = xys[root];
			AddEs(0,5000,x0,y0,NM,root);
			for ( int u=N; u<NM; u++ ) {
				if ( assigns[u]==-1 ) continue;
				var (v0,v1) = gs[u];
				if ( v0!=-1 ) AddEs( u, v0 );
				if ( v1!=-1 ) AddEs( u, v1 );
			}
		}
		public void RebuildRs() {
			for ( int i=0; i<NM; i++ ) rs[i].Clear();
			for ( int i=N; i<NM; i++ ) {
				var (v0,v1) = gs[i];
				if ( v0!=-1 ) rs[v0].Add( i );
				if ( v1!=-1 ) rs[v1].Add( i );
			}
			rs[root].Add( -1 ); // 搬入口を追加
		}
		public void RebuildAll() {
			RebuildRs();
			RebuildEs();
			RebuildOrder();
			RebuildRootLeafProbDrain();
			InvalidateProbs();
			InvalidateDrains();
		}

		//@@ Invalidate -------------------------------------------------------
		public void InvalidateProbs( int u0=-1 ) {
			if ( u0==-1 ) {
				Array.Fill( invalidProbs, 1 );
			} else {
				visId++;
				que.Clear();
				que.Push( u0 );
				vis[u0] = visId;
				invalidProbs[u0] = 1;
				while ( que.Exists ) {
					var u = que.Pop();
					var (v0,v1) = gs[u];
					if ( v0!=-1 && vis[v0]!=visId && invalidProbs[v0]==0 ) {
						que.Push( v0 );
						vis[v0] = visId;
						invalidProbs[v0] = 1;
					}
					if ( v1!=-1 && vis[v1]!=visId && invalidProbs[v1]==0 ) {
						que.Push( v1 );
						vis[v1] = visId;
						invalidProbs[v1] = 1;
					}
				}
			}
		}
		public void InvalidateDrains( int u0=-1 ) {
			if ( u0==-1 ) {
				Array.Fill( invalidDrains, 1 );
			} else {
				visId++;
				que.Clear();
				que.Push( u0 );
				vis[u0] = visId;
//				invalidDrains[u0] = 1;
				while ( que.Exists ) {
					var u = que.Pop();
					var rs0 = rs[u];
					for ( int i=0; i<rs0.Count; i++ ) {
						var v = rs0[i];
						if ( v!=-1 && vis[v]!=visId && invalidDrains[v]==0 ) {
							que.Push( v );
							vis[v] = visId;
							invalidDrains[v] = 1;
						}
					}
				}
			}
		}
		//@@ Propagate -------------------------------------------------------
		public void PropagateProbsDiff( int u0, int k0, int k1, int swap ) {
			var (v0,v1) = gs[u0];
			var tbl00 = Terminal.Probs0[k0];
			var tbl01 = Terminal.Probs1[k0];
			var tbl10 = Terminal.Probs0[k1];
			var tbl11 = Terminal.Probs1[k1];
			var probs0 = probs[u0];
			var probsv0 = probs[v0];
			var probsv1 = probs[v1];
			if ( swap==1 ) {
				for ( int j=0; j<N; j++ ) {
					probsv0[j] += probs0[j] * ( tbl11[j] - tbl00[j] );
					probsv1[j] += probs0[j] * ( tbl10[j] - tbl01[j] );
				}
			} else {
				for ( int j=0; j<N; j++ ) {
					probsv0[j] += probs0[j] * ( tbl10[j] - tbl00[j] );
					probsv1[j] += probs0[j] * ( tbl11[j] - tbl01[j] );
				}
			}
		}

		//@@ Update -------------------------------------------------------
		public void UpdateProbs() {
			for ( int i=0; i<orders.Count; i++ ) {
				var u =orders[i];
				if ( invalidProbs[u]==1 && u!=root ) Array.Fill( probs[u], 0.0 );
			}
			for ( int i=orders.Count-1; 0<=i; i-- ) {
				var u = orders[i];
				if ( u<N ) continue;
				if ( u<N ) continue;
				var k = assigns[u];
				var tbl0 = Terminal.Probs0[k];
				var tbl1 = Terminal.Probs1[k];
				var (v0,v1) = gs[u];
				var probs0 = probs[u];
				if ( v0==v1 ) {
					if ( invalidProbs[v0]==1 ) {
						for ( int j=0; j<N; j++ ) {
							probs[v0][j] += probs0[j];
						}
					}
				} else {
					if ( invalidProbs[v0]==1 ) {
						for ( int j=0; j<N; j++ ) {
							probs[v0][j] += probs0[j] * tbl0[j];
						}
					}
					if ( invalidProbs[v1]==1 ) {
						for ( int j=0; j<N; j++ ) {
							probs[v1][j] += probs0[j] * tbl1[j];
						}
					}
				}
				invalidProbs[u] = 0;
			}
		}
		public void UpdateDrains() {
			for ( int i=0; i<orders.Count; i++ ) {
				var u = orders[i];
				if ( invalidDrains[u]==0 || u<N ) continue;
				var k = assigns[u];
				var tbl0 = Terminal.Probs0[k];
				var tbl1 = Terminal.Probs1[k];
				var (v0,v1) = gs[u];
				var drains0 = drains[u];
				var drainsv0 = drains[v0];
				var drainsv1 = drains[v1];
//				var deltas0 = deltas[u];
//				var deltasv0 = deltas[v0];
//				var deltasv1 = deltas[v1];
				if ( v0==v1 ) {
					for ( int j=0; j<N; j++ ) {
						drains0[j] = drainsv0[j];
					}
				} else {
					for ( int j=0; j<N; j++ ) {
						drains0[j] = drainsv0[j] * tbl0[j] + drainsv1[j] * tbl1[j];
//						deltas0[j] = deltasv0[j] * tbl0[j] + deltasv1[j] * tbl1[j];
					}
				}

//				var probs0 = probs[u];
//				var sum = 0.0;
//				for ( int j=0; j<N; j++ ) {
//					sum += ( 1.0 - drains0[j] ) * probs0[j];
//				}
//				errors[u] = sum;

				invalidDrains[u] = 0;
			}
		}

		public void UpdateDrains0() {
			for ( int i=0; i<orders.Count; i++ ) {
				var u = orders[i];
//				if ( invalidDrains[u]==0 || u<N ) continue;
				if ( u<N ) continue;
				var (newK,difScore) = OptSorter( u );

				var k = assigns[u];
				var tbl0 = Terminal.Probs0[k];
				var tbl1 = Terminal.Probs1[k];
				var (v0,v1) = gs[u];
				var drains0 = drains[u];
				var drainsv0 = drains[v0];
				var drainsv1 = drains[v1];
				for ( int j=0; j<N; j++ ) {
					drains0[j] = drainsv0[j] * tbl0[j] + drainsv1[j] * tbl1[j];
				}
			}
		}

		//@@ Eval -------------------------------------------------------
		public int Eval() {

			UpdateProbs();

//			for ( int u=0; u<N; u++ ) {
//				for ( int i=0; i<N; i++ ) {
//					var d = drains[u][i] - probs[u][i];
//					deltas[u][i] = Math.Min( d, 0.0 );
//				}
//			}

			UpdateDrains();

			score = 0.0;
			for ( int i=0; i<N; i++ ) {
				var p = assigns[i];
				score += ( 1.0 - probs[i][p] );
			}
			score = Math.Round( 1.0e9 * score / N );
			return (int)score;
		}

		//@@ NextTraSwitchBelt -------------------------------------------------------
		//@@ NextTraChangeSorter -------------------------------------------------------
		public (int,double) OptSorter( int u ) {
			var (v0,v1) = gs[u];
			var probs0 = probs[u];
			var drains0 = drains[u];
			var drainsv0 = drains[v0];
			var drainsv1 = drains[v1];
			var maxVal = 0.0;
			var maxIdx = 0;
			for ( int i=0; i<K; i++ ) {
				var tbl0 = new Span<double>( Terminal.Probs0[i] );
				var tbl1 = new Span<double>( Terminal.Probs1[i] );
				var sum0 = 0.0;
				var sum1 = 0.0;
				for ( int j=0; j<N; j++ ) {
					sum0 += ( drainsv0[j] * tbl0[j] + drainsv1[j] * tbl1[j] ) * probs0[j];
					sum1 += ( drainsv1[j] * tbl0[j] + drainsv0[j] * tbl1[j] ) * probs0[j];
				}
				if ( maxVal<sum0) {
					maxVal = sum0;
					maxIdx = i;
				}
				if ( maxVal<sum1 ) {
					maxVal = sum1;
					maxIdx = i;
				}
			}
			var curSum = 0.0;
			for ( int i=0; i<N; i++ ) {
				curSum += drains0[i] * probs0[i];
			}
			var difScore = 1.0e9*(maxVal-curSum)/N;
			return (maxIdx,difScore);
		}

		public Transition NextTraChangeSorter( int u, bool mergeBelt ) {
			var (v0,v1) = gs[u];
			if ( v0==-1 || v1==-1 ) return null;
			var probs0 = probs[u];
			var drains0 = drains[u];
			var drainsv0 = drains[v0];
			var drainsv1 = drains[v1];

			var maxVal = 0.0;
			var maxTyp = 0;
			var maxSum = 0.0;
			var maxIdx = 0;
			var maxSwp = 0;
			var maxSin = -1;
			var curSorterIdx = assigns[u];

			var spv0 = new Span<double>( cachet0v0 );
			var spv1 = new Span<double>( cachet0v1 );
			for ( int i=0; i<N; i++ ) {
				spv0[i] = drainsv0[i] * probs0[i];
				spv1[i] = drainsv1[i] * probs0[i];
			}
			for ( int i=0; i<K; i++ ) {
				if ( !Prm.prmNextTraAcceptSelf && curSorterIdx==i ) continue;
				var jitter = Prm.prmNextTraTGain * temperature * rand.P();
				var tbl0 = new Span<double>( Terminal.Probs0[i] );
				var tbl1 = new Span<double>( Terminal.Probs1[i] );
				var sum0 = 0.0;
				var sum1 = 0.0;
				for ( int j=0; j<N; j++ ) {
//					sum0 += ( drainsv0[j] * tbl0[j] + drainsv1[j] * tbl1[j] ) * probs0[j];
//					sum1 += ( drainsv1[j] * tbl0[j] + drainsv0[j] * tbl1[j] ) * probs0[j];
					sum0 += spv0[j] * tbl0[j] + spv1[j] * tbl1[j];
					sum1 += spv1[j] * tbl0[j] + spv0[j] * tbl1[j];
				}
				if ( maxVal<sum0+jitter) {
					maxVal = sum0+jitter;
					maxSum = sum0;
					maxIdx = i;
					maxSwp = 0;
				}
				if ( maxVal<sum1+jitter ) {
					maxVal = sum1+jitter;
					maxSum = sum1;
					maxIdx = i;
					maxSwp = 1;
				}
			}

			if ( mergeBelt && v0!=v1 ) {
				var jitter = temperature * rand.P();
				var sum0 = 0.0;
				var sum1 = 0.0;
				for ( int j=0; j<N; j++ ) {
					sum0 += drainsv0[j] * probs0[j];
					sum1 += drainsv1[j] * probs0[j];
				}
				if ( 1<rs[v1].Count && maxVal<sum0+jitter ) {
					maxTyp = 1;
					maxSum = sum0;
					maxSin = 0;
				}
				if ( 1<rs[v0].Count && maxVal<sum1+jitter ) {
					maxTyp = 1;
					maxSum = sum1;
					maxSin = 1;
				}
			}

			var curSum = 0.0;
			for ( int i=0; i<N; i++ ) {
				curSum += drains0[i] * probs0[i];
			}
			var tra = Transition.pool.Alloc();
			tra.type       = maxTyp;
			tra.target0    = u;
			tra.sorterIdx0 = maxIdx;
			tra.swap       = maxSwp;
			tra.single     = maxSin;
			tra.score      = score - 1.0e9*(maxSum-curSum)/N;
			return tra;
		}

		public Transition NextTraSwitchBelt( int u ) {
			if ( u<N ) return null;
			var (v0,v1) = gs[u];

			var xys = Terminal.Xys;
			var nears = Terminal.NearsA;

			var probs0 = probs[u];
			var drains0 = drains[u];
			var curSum = 0.0;
			for ( int i=0; i<N; i++ ) {
				curSum += drains0[i] * probs0[i];
			}
			var near = nears[u];
			var drainsv0 = drains[v0];
			var drainsv1 = drains[v1];
//			var deltasv0 = deltas[v0];
//			var deltasv1 = deltas[v1];
			var vx = -1;
			var maxVal = 0.0;
			var maxSum = 0.0;
			var maxIdx = 0;
			var maxSin = 0;
			var maxSwp = 0;
			var maxCnt = 0;
			var curSorterIdx = assigns[u];
			for ( var i=0; i<Prm.prmNextTraSwitchMaxNear; i++ ) {
				var v = near[i];
				if ( v==v0 || v==v1 || assigns[v]==-1 ) continue;
				if ( Intersects( u, v ) ) continue;
				if ( Util.IsCycle( gs, u, v ) ) continue;
				var drainsvx = drains[v];
//				var deltasvx = deltas[v];
				for ( int k=0; k<K; k++ ) {
					double jitter = 0.0;
					var tbl0 = new Span<double>( Terminal.Probs0[k] );
					var tbl1 = new Span<double>( Terminal.Probs1[k] );
					var sum0 = 0.0;
					var sum1 = 0.0;
					var sum2 = 0.0;
					var sum3 = 0.0;
					var sum4 = 0.0;
					for ( int j=0; j<N; j++ ) {
//						sum0 += ( drainsvx[j] * tbl0[j] + drainsv1[j] * tbl1[j] ) * probs0[j];
//						sum1 += ( drainsv0[j] * tbl0[j] + drainsvx[j] * tbl1[j] ) * probs0[j];
//						sum2 += ( drainsvx[j] * tbl1[j] + drainsv1[j] * tbl0[j] ) * probs0[j];
//						sum3 += ( drainsv0[j] * tbl1[j] + drainsvx[j] * tbl0[j] ) * probs0[j];
						var t0 = drainsvx[j] * tbl0[j];
						var t1 = drainsvx[j] * tbl1[j];
						var t2 = drainsv0[j] * tbl1[j];
						var t3 = drainsv1[j] * tbl0[j];
						var t4 = drainsv1[j] * tbl1[j];
						var t5 = drainsv0[j] * tbl0[j];

						sum0 += ( t0 + t4 ) * probs0[j];
						sum1 += ( t5 + t1 ) * probs0[j];
						sum2 += ( t1 + t3 ) * probs0[j];
						sum3 += ( t2 + t0 ) * probs0[j];
						sum4 += ( t2 + t3 ) * probs0[j];
					}
					if ( maxVal<sum0+jitter ) {
						vx = v;
						maxVal = sum0+jitter;
						maxSum = sum0;
						maxIdx = k;
						maxSin = 0;
						maxSwp = 0;
						maxCnt++;
					}
					if ( maxVal<sum1+jitter ) {
						vx = v;
						maxVal = sum1+jitter;
						maxSum = sum1;
						maxIdx = k;
						maxSin = 1;
						maxSwp = 0;
						maxCnt++;
					}
					if ( maxVal<sum2+jitter ) {
						vx = v;
						maxVal = sum2+jitter;
						maxSum = sum2;
						maxIdx = k;
						maxSin = 0;
						maxSwp = 1;
						maxCnt++;
					}
					if ( maxVal<sum3+jitter ) {
						vx = v;
						maxVal = sum3+jitter;
						maxSum = sum3;
						maxIdx = k;
						maxSin = 1;
						maxSwp = 1;
						maxCnt++;
					}
					if ( maxVal<sum4+jitter ) {
						vx = v;
						maxVal = sum4+jitter;
						maxSum = sum4;
						maxIdx = k;
						maxSin = -1;
						maxSwp = 1;
						maxCnt++;
					}
				}
				if ( v0!=v1 ) {
					var sum5 = 0.0;
					for ( int j=0; j<N; j++ ) {
						sum5 += drainsvx[j] * probs0[j];
					}
					if ( maxVal<sum5 && 1<rs[v0].Count && 1<rs[v1].Count ) {
						vx = v;
						maxVal = sum5;
						maxSum = sum5;
						maxSin = -2;
						maxCnt++;
					}
				}
				if ( curSum<maxSum && Prm.prmNextTraSwitchMaxTry<=maxCnt ) break;
			}

			if ( v0!=v1 ) {
				var sum6 = 0.0;
				var sum7 = 0.0;
				for ( int j=0; j<N; j++ ) {
					sum6 += drainsv0[j] * probs0[j];
					sum7 += drainsv1[j] * probs0[j];
				}
				if ( maxVal<sum6 && 1<rs[v1].Count ) {
					vx = v0;
					maxVal = sum6;
					maxSum = sum6;
					maxSin = -3;
				}
				if ( maxVal<sum7 && 1<rs[v0].Count ) {
					vx = v1;
					maxVal = sum7;
					maxSum = sum7;
					maxSin = -4;
				}
			}

			if ( vx!=-1 ) {
				var tra = Transition.pool.Alloc();
				tra.type       = 2;
				tra.target0    = u;
				tra.sub2       = vx;
				tra.sorterIdx0 = maxIdx;
				tra.single     = maxSin;
				tra.swap       = maxSwp;
				tra.score      = score - 1.0e9*(maxSum-curSum)/N;
				return tra;
			}
			return null;
		}
		public Transition NextTraSwitchBelt2( int u ) {
			if ( u<N ) return null;
			var (v0,v1) = gs[u];

			var xys = Terminal.Xys;
			var nears = Terminal.NearsA;

			var probs0 = probs[u];
			var drains0 = drains[u];
			var curSum = 0.0;
			for ( int i=0; i<N; i++ ) {
				curSum += drains0[i] * probs0[i];
			}
			var near = nears[u];
			var drainsv0 = drains[v0];
			var drainsv1 = drains[v1];
			var maxVal = 0.0;
			var maxIdx = 0;
			var maxSin = 0;
			var maxV0  = -1;
			var maxV1  = -1;
			var maxVx  = -1;
			var maxCnt = 0;
			var vz = -1;
			var curSorterIdx = assigns[u];

			var en01 = (v0!=v1) ? true                               : true             ;
			var en0x = (v0!=v1) ? (1<rs[v1].Count)                   : true             ;
			var en10 = (v0!=v1) ? true                               : false            ;
			var en1x = (v0!=v1) ? (1<rs[v0].Count)                   : false            ;
			var enx0 = (v0!=v1) ? (1<rs[v1].Count)                   : true             ;
			var enx1 = (v0!=v1) ? (1<rs[v0].Count)                   : false            ;
			var en00 = (v0!=v1) ? (1<rs[v1].Count)                   : false            ;
			var en11 = (v0!=v1) ? (1<rs[v0].Count)                   : false            ;
			var enxx = (v0!=v1) ? (1<rs[v0].Count && 1<rs[v1].Count) : (2<rs[v0].Count) ;
			var enxz = enxx;
			var enzx = enxx;

			var spt0v0 = new Span<double>( cachet0v0 );
			var spt0v1 = new Span<double>( cachet0v1 );
			var spt1v0 = new Span<double>( cachet1v0 );
			var spt1v1 = new Span<double>( cachet1v1 );
			var spt0vz = new Span<double>( cachet0vx );
			var spt1vz = new Span<double>( cachet1vx );
			for ( int k=0; k<K; k++ ) {
				var tbl0 = Terminal.Probs0[k];
				var tbl1 = Terminal.Probs1[k];
				var cache00 = 0.0;
				var cache01 = 0.0;
				var cache10 = 0.0;
				var cache11 = 0.0;
				for ( int j=0; j<N; j++ ) {
					var a0 = drainsv0[j] * probs0[j];
					var a1 = drainsv1[j] * probs0[j];
					cache00 += a0 * tbl0[j];
					cache01 += a1 * tbl0[j];
					cache10 += a0 * tbl1[j];
					cache11 += a1 * tbl1[j];
				}
				spt0v0[k] = cache00;
				spt0v1[k] = cache01;
				spt1v0[k] = cache10;
				spt1v1[k] = cache11;
			}
			for ( var i=0; i<Prm.prmNextTraSwitchMaxNear; i++ ) {
				var v = near[i];
				if ( v==v0 || v==v1 || assigns[v]==-1 ) continue;
				if ( Intersects( u, v ) ) continue;
				if ( Util.IsCycle( gs, u, v ) ) continue;
				var drainsvx = drains[v];
				for ( int k=0; k<K; k++ ) {
					var tbl0 = Terminal.Probs0[k];
					var tbl1 = Terminal.Probs1[k];
					var sum01 = 0.0;
					var sum0x = 0.0;
					var sum10 = 0.0;
					var sum1x = 0.0;
					var sumx0 = 0.0;
					var sumx1 = 0.0;
					var sumxz = 0.0;
					var sumzx = 0.0;
					var spt0vx = 0.0;
					var spt1vx = 0.0;
					for ( int j=0; j<N; j++ ) {

//						var t0v0 = drainsv0[j] * tbl0[j];
//						var t0v1 = drainsv1[j] * tbl0[j];
//						var t0vx = drainsvx[j] * tbl0[j];
//						var t1v0 = drainsv0[j] * tbl1[j];
//						var t1v1 = drainsv1[j] * tbl1[j];
//						var t1vx = drainsvx[j] * tbl1[j];

//						sum01 += ( t0v0 + t1v1 ) * probs0[j];
//						sum0x += ( t0v0 + t1vx ) * probs0[j];
//						sum10 += ( t0v1 + t1v0 ) * probs0[j];
//						sum1x += ( t0v1 + t1vx ) * probs0[j];
//						sumx0 += ( t0vx + t1v0 ) * probs0[j];
//						sumx1 += ( t0vx + t1v1 ) * probs0[j];

						var a0 = drainsvx[j] * probs0[j];
						spt0vx += a0 * tbl0[j];
						spt1vx += a0 * tbl1[j];
					}

					sum01 = spt0v0[k] + spt1v1[k];
					sum0x = spt0v0[k] + spt1vx;
					sum10 = spt0v1[k] + spt1v0[k];
					sum1x = spt0v1[k] + spt1vx;
					sumx0 = spt0vx    + spt1v0[k];
					sumx1 = spt0vx    + spt1v1[k];
					sumxz = spt0vx    + spt1vz[k];
					sumzx = spt0vz[k] + spt1vx;

//					spt0vz[k] = spt0vx;
//					spt1vz[k] = spt1vx;

//					if ( en01 && maxVal<sum01 ) {
//						maxVal = sum01;
//						maxIdx = k;
//						maxV0  = v0;
//						maxV1  = v1;
//						maxVx  = v;
//						maxSin = 0;
//						maxCnt++;
//					}
					if ( en0x && maxVal<sum0x ) {
						maxVal = sum0x;
						maxIdx = k;
						maxV0  = v0;
						maxV1  = v;
						maxVx  = v;
						maxSin = 1;
						maxCnt++;
					}
					if ( en10 && maxVal<sum10 ) {
						maxVal = sum10;
						maxIdx = k;
						maxV0  = v1;
						maxV1  = v0;
						maxVx  = v;
						maxSin = 0;
						maxCnt++;
					}
					if ( en1x && maxVal<sum1x ) {
						maxVal = sum1x;
						maxIdx = k;
						maxV0  = v1;
						maxV1  = v;
						maxVx  = v;
						maxSin = 1;
						maxCnt++;
					}
					if ( enx0 && maxVal<sumx0 ) {
						maxVal = sumx0;
						maxIdx = k;
						maxV0  = v;
						maxV1  = v0;
						maxVx  = v;
						maxSin = 1;
						maxCnt++;
					}
					if ( enx1 && maxVal<sumx1 ) {
						maxVal = sumx1;
						maxIdx = k;
						maxV0  = v;
						maxV1  = v1;
						maxVx  = v;
						maxSin = 1;
						maxCnt++;
					}
					if ( vz!=-1 && enxz && maxVal<sumxz ) {
						maxVal = sumxz;
						maxIdx = k;
						maxV0  = v;
						maxV1  = vz;
						maxVx  = v;
						maxSin = 2;
						maxCnt++;
					}
					if ( vz!=-1 && enzx && maxVal<sumzx ) {
						maxVal = sumzx;
						maxIdx = k;
						maxV0  = vz;
						maxV1  = v;
						maxVx  = v;
						maxSin = 2;
						maxCnt++;
					}
				}
				vz = v;
				if ( enxx ) {
					var sumxx = 0.0;
					for ( int j=0; j<N; j++ ) {
						sumxx += drainsvx[j] * probs0[j];
					}
					if ( maxVal<sumxx ) {
						maxVal = sumxx;
						maxIdx = curSorterIdx;
						maxV0  = v;
						maxV1  = v;
						maxVx  = v;
						maxSin = 1;
						maxCnt++;
					}
				}
				if ( curSum<maxVal && Prm.prmNextTraSwitchMaxTry<=maxCnt ) break;
			}

			if ( en00 ) {
				var sum00 = 0.0;
				for ( int j=0; j<N; j++ ) {
					sum00 += drainsv0[j] * probs0[j];
				}
				if ( maxVal<sum00 ) {
					maxVal = sum00;
					maxIdx = curSorterIdx;
					maxV0  = v0;
					maxV1  = v0;
					maxVx  = -1;
					maxSin = 0;
					maxCnt++;
				}
			}
			if ( en11 ) {
				var sum11 = 0.0;
				for ( int j=0; j<N; j++ ) {
					sum11 += drainsv1[j] * probs0[j];
				}
				if ( maxVal<sum11 ) {
					maxVal = sum11;
					maxIdx = curSorterIdx;
					maxV0  = v1;
					maxV1  = v1;
					maxVx  = -1;
					maxSin = 0;
					maxCnt++;
				}
			}

			if ( maxCnt!=0 ) {
				var tra = Transition.pool.Alloc();
				tra.type       = 7;
				tra.target0    = u;
				tra.sub0       = maxV0;
				tra.sub1       = maxV1;
				tra.sub2       = maxVx;
				tra.sorterIdx0 = maxIdx;
				tra.single     = maxSin;
				tra.score      = score - 1.0e9*(maxVal-curSum)/N;
				return tra;
			}
			return null;
		}

		//@@ NextTraSwapProcessor -------------------------------------------------------
		public Transition NextTraSwapProcessor() {
			var maxVal = 0.0;
			var maxIdx0 = 0;
			var maxIdx1 = 0;
			for ( int i=0; i<N; i++ ) {
				var u = assigns[i];
				var jitter = 0.1 * rand.P();
				for ( int j=i+1; j<N; j++ ) {
					var v = assigns[j];
					var now = probs[u][i]+probs[v][j];
					var swp = probs[v][i]+probs[u][j];
					var sum = swp - now + jitter;
					if ( maxVal<sum ) {
						maxVal = sum;
						maxIdx0 = i;
						maxIdx1 = j;
					}
				}
			}

			var tra = Transition.pool.Alloc();
			tra.type      = 3;
			tra.target0   = maxIdx0;
			tra.sub0      = maxIdx1;
			tra.score     = score - 1.0e9*maxVal/N;
			return tra;
		}

		//@@ NextTraPlaceSorter -------------------------------------------------------
		public Transition NextTraPlaceSorter( int u ) {
			var nears = Terminal.NearsA;
			var near = nears[u];
			var tarIdx0 = -1;
			var tarIdx1 = -1;
			for ( var i=0; i<Prm.prmNextTraSwitchMaxNear; i++ ) {
				var v = near[i];
				if ( v<N || assigns[v]==-1 ) continue;
				if ( Intersects( v, u ) ) continue; // 向きに注意 v -> u -> v0 を探す
				var (v0,v1) = gs[v];
				if ( !Intersects( u, v0 ) ) {
					tarIdx0 = v;
					tarIdx1 = v0;
					goto END;
				} else if ( !Intersects( u, v1 ) ) {
					tarIdx0 = v;
					tarIdx1 = v1;
					goto END;
				}
			}

			END:
			if ( tarIdx0==-1 ) return null;
			var tra = Transition.pool.Alloc();
			tra.type       = 4;
			tra.target0    = u;
			tra.sub0       = tarIdx0;
			tra.sub1       = tarIdx1;
			tra.score      = score;
			return tra;
		}

		//@@ NextTra -------------------------------------------------------
		public Transition NextTra() {
			Transition tra = null;

			var u = PopShuffleSorterIdx( );

			var retryNum = 0;
			while ( assigns[u]==-1 ) {
				// 分別器の設置を試みる
				if ( Prm.prmNextTraPlaceSorter ) {
					tra = NextTraPlaceSorter( u );
					if ( tra!=null ) return tra;
				}

				retryNum++;
				if ( NM<retryNum ) {
					tra = Transition.pool.Alloc();
					tra.type  = -1;
					tra.score = score - 1;
					return tra;
				}
				u = PopShuffleSorterIdx();
			}


			// 処理機の位置を入れ替えを試みる
			if ( Prm.prmNextTraSwapProc ) {
				tra = NextTraSwapProcessor();
				return tra;
			}

//			if ( Prm.prmNextTraSwitchBelt && rand.B( Prm.prmNextTraSwitchBeltP ) ) {
//				tra = NextTraSwitchBelt( u );
//				if ( tra!=null ) {
//					return tra;
//				}
//			}

			// 経路＋分別器の種類変更を試みる
//Solver.debugCnt0++;
			if ( Prm.prmNextTraSwitchBelt ) {
				tra = NextTraSwitchBelt2( u );
				if ( tra!=null ) {
//Solver.debugCnt1++;
					return tra;
				}
			}

			// 分別器の種類変更を試みる
			if ( gs[u].Item1!=gs[u].Item2 ) {
				tra = NextTraChangeSorter( u, Prm.prmNextTraMergeBelt );
				if ( tra!=null ) {
					return tra;
				}
			}

			// 全部失敗したら何もしない遷移にする
			tra = Transition.pool.Alloc();
			tra.type  = -1;
			tra.score = score - 1;
			return  tra;
		}

		//@@ ApplyTra -------------------------------------------------------
		public void ApplyTra( Transition tra, bool update=true ) {
			if ( tra.type==-1 ) return;

			var u0 = tra.target0;
			if ( tra.type==0 ) {
				// NextTraChangeSorter
				if ( update ) InvalidateProbs( u0 );
				if ( update ) InvalidateDrains( u0 );

				assigns[u0] = tra.sorterIdx0;
				if ( tra.swap==1 ) {
					var (v0,v1)=gs[u0];
					gs[u0] = (v1,v0);
				}
			} else if ( tra.type==1 ) {
				// NextTraChangeSorter
				if ( update ) InvalidateProbs( u0 );
				if ( update ) InvalidateDrains( u0 );

				var (v0,v1)=gs[u0];
				if ( tra.single==0 ) {
					gs[u0] = (v0,v0);
					rs[v0].Add( u0 );
					rs[v1].Remove( u0 );
					RemoveEs( u0, v1 );
					AddEs( u0, v0 );
				} else if ( tra.single==1 ) {
					gs[u0] = (v1,v1);
					rs[v0].Remove( u0 );
					rs[v1].Add( u0 );
					RemoveEs( u0, v0 );
					AddEs( u0, v1 );
				}

			} else if ( tra.type==2 ) {
				// NextTraSwitchBelt
				if ( update ) InvalidateProbs( u0 );
				if ( update ) InvalidateDrains( u0 );
				assigns[u0] = tra.sorterIdx0;
				var vx = tra.sub2;
				if ( 0<=tra.single ) {
					var vz = 0;
					var (v0,v1) = gs[u0];
					if ( tra.single==0 ) {
						vz = v0;
						v0 = vx;
					} else {
						vz = v1;
						v1 = vx;
					}
					assigns[u0] = tra.sorterIdx0;
					gs[u0] = (v0,v1);
					rs[vz].Remove( u0 );
					RemoveEs( u0, vz );
					rs[vx].Add( u0 );
					AddEs( u0, vx );
				} else if ( tra.single==-2 ) {
					var (v0,v1) = gs[u0];
					gs[u0] = (vx,vx);
					rs[v0].Remove( u0 );
					rs[v1].Remove( u0 );
					rs[vx].Add( u0 );
					rs[vx].Add( u0 );
					RemoveEs( u0, v0 );
					RemoveEs( u0, v1 );
					AddEs( u0, vx );
					AddEs( u0, vx );
				} else if ( tra.single==-3 ) {
					var (v0,v1) = gs[u0];
					gs[u0] = (v0,v0);
					rs[v1].Remove( u0 );
					rs[v0].Add( u0 );
					RemoveEs( u0, v1 );
					AddEs( u0, v0 );
				} else if ( tra.single==-4 ) {
					var (v0,v1) = gs[u0];
					gs[u0] = (v1,v1);
					rs[v0].Remove( u0 );
					rs[v1].Add( u0 );
					RemoveEs( u0, v0 );
					AddEs( u0, v1 );
				}
				if ( tra.swap==1 ) {
					var (v0,v1)=gs[u0];
					gs[u0] = (v1,v0);
				}

				// ルート変更に伴うに更新
				if ( tra.single!=-1 ) {
					RebuildOrder();
					if ( update ) InvalidateProbs( vx );
					if ( update ) InvalidateDrains( vx );
				}

			} else if ( tra.type==7 ) {
				// NextTraSwitchBelt2
				if ( update ) InvalidateProbs( u0 );
				if ( update ) InvalidateDrains( u0 );

				assigns[u0] = tra.sorterIdx0;
				var w0 = tra.sub0;
				var w1 = tra.sub1;
				var wz = tra.sub2;

				var (v0,v1) = gs[u0];
				rs[v0].Remove( u0 );
				rs[v1].Remove( u0 );
				RemoveEs( u0, v0 );
				RemoveEs( u0, v1 );

				gs[u0] = (w0,w1);
				rs[w0].Add( u0 );
				rs[w1].Add( u0 );
				AddEs( u0, w0 );
				AddEs( u0, w1 );

				// ルート変更に伴うに更新
//				if ( tra.single==1 ) {
				if ( wz!=-1 ) {
					RebuildOrder();
					if ( update ) InvalidateProbs( wz );
					if ( update ) InvalidateDrains( wz );
					if ( tra.single==2 ) {
						if ( update ) InvalidateProbs( w0 );
						if ( update ) InvalidateDrains( w1 );
					}
				}

			} else if ( tra.type==3 ) {
				// NextTraSwapProcessor
				var u = tra.target0;
				var v = tra.sub0;
				var w = assigns[u];
				assigns[u] = assigns[v];
				assigns[v] = w;
				if ( update ) InvalidateDrains( u );
				if ( update ) InvalidateDrains( v );
				RebuildRootLeafProbDrain();

			} else if ( tra.type==4 ) {
				// NextTraPlaceSorter
				// v -> u -> vx
				var u  = tra.target0;
				var v  = tra.sub0;
				var vx = tra.sub1;

				var (v0,v1)=gs[v];
				if ( vx==v0 ) {
					// v -> u -> v0
					gs[v]=(u,v1);
					gs[u]=(vx,vx);
					rs[vx].Remove(v);
					rs[u].Add( v );
					rs[vx].Add( u );
					rs[vx].Add( u );

					AddEs( v, u );
					AddEs( u, vx );
					AddEs( u, vx );
					RemoveEs( v, vx );

				} else {
					// v -> u -> v1
					gs[v]=(v0,u);
					gs[u]=(vx,vx);
					rs[u].Remove(v);
					rs[u].Add( v );
					rs[vx].Add( u );
					rs[vx].Add( u );

					AddEs( v, u );
					AddEs( u, vx );
					AddEs( u, vx );
					RemoveEs( v, vx );
				}

				assigns[u] = rand.I(K);
				if ( update ) InvalidateProbs( v );
				if ( update ) InvalidateDrains( v );
			}
			if ( update ) Eval();
		}

		//@@ GenRouteToAny -------------------------------------------------------
		public bool GenRouteToAnyRec( int u, Stack<int> stk0, List<(int,int,int,int)> es0, ref int cnt ) {
			if ( 1000<cnt ) return false;
			var xys = Terminal.Xys;
			var nears = Terminal.NearsA;
			var near = nears[u];
			for ( int i=0; i<8; i++ ) {
				cnt++;
				var v = near[i];
				if ( vis[v]==visId ) continue;
				if ( Intersects( u, v ) ) continue;
				if ( Util.IntersectsBrute( es0, u, v ) ) continue; // 探索中のベルトに衝突してるか判定

				if ( assigns[v]!=-1 ) {
					stk0.Push( v );
					return true;
				}

				vis[v] = visId;
				stk0.Push( v );
				es0.Add((xys[u][0],xys[u][1],xys[v][0],xys[v][1]));
				if ( GenRouteToAnyRec( v, stk0, es0, ref cnt ) ) {
					return true;
				}
				vis[v] = -1;
				stk0.Pop();
				es0.RemoveAt(es0.Count-1);
			}
			return false;
		}

		public List<int> GenRouteToAny( int u ) {
			visId++;
			vis[u] = visId;
			var stk =new Stack<int>();
			var es0 =new List<(int,int,int,int)>();
			var cnt = 0;
			GenRouteToAnyRec( u, stk, es0, ref cnt );
			return stk.Reverse().ToList();
		}
		public (int,int) GenRouteSingleFromRoot( List<int[]> path, int size ) {
			var endPos = -1;
			var genSize = 0;
			var sizs0 = Math.Min( size, path.Count -1 );
			for ( int i=0; i<sizs0; i++ ) {
				var u = path[i][2];
				var v = path[i+1][2];
				if ( v<N ) break; // 処理装置があればキャンセル
				if ( i==0 ) {
					root = v;
					rs[v].Add( -1 );
					AddEs(0,5000,path[i+1][0],path[i+1][1],NM,v);
				} else {
					PlaceBeltWithoutCheck( u, v, v );
				}
				assigns[v] = rand.I(K);
				endPos = v;
				genSize++;
			}
			return (endPos,genSize);
		}

		//@@ GenRoot -------------------------------------------------------
		//@@ 初期サンプル作成
		public static Sample GenRoot( int type, int seed ) {
			var xys = Terminal.Xys;
			var o = Sample.pool.Alloc();
			o.Clear();
			o.seed = seed + 1000 * type;
//			o.zoom = 0;

			// 搬入口から凸包ベルトコンベアを引く
			var startPos = -1;
			if ( type!=0  ) {
				var path = ( type==1 ) ? Terminal.ConvexRXys : ( type==2 ) ? Terminal.ConvexLXys : Terminal.HorizenXys;
				var reqSize = seed + 1;
				(startPos,var genSize) = o.GenRouteSingleFromRoot( path, reqSize );
				seed = ( type==3 ) ? -1 : reqSize-genSize-1;
				if ( 0<=seed ) return null;
			} else {
				// 搬入口の近くを初期配置にする
				o.root = Terminal.Nears0[NM][seed];
				o.rs[o.root].Add( -1 );
				var (x0,y0) = xys[o.root];
				o.AddEs(0,5000,x0,y0,NM,o.root);
				startPos = o.root;
			}


			// スタート地点からテキトーに分別器を貪欲に配置
			var nears = Terminal.NearsA;
			var maxNear = (int)Prm.InterpolateXY( M, 50, 64, 1000, 32 );
			que.Clear();
			que.Push( startPos );
			visId++;
			while ( que.Exists ) {
				var u = que.Pop();
				if ( vis[u]==visId ) continue;

				var near = nears[u];
				var v0 = -1;
				var v1 = -1;
				var i = 0;
				for ( ; i<maxNear; i++ ) {
					var nx = near[i];
					if ( o.Intersects( u, nx ) ) continue;
					if ( Util.IsCycle( o.gs, u, nx ) ) continue;
					v0 = nx;
					break;
				}
				i++;
				for ( ; i<maxNear; i++ ) {
					var nx = near[i];
					if ( o.Intersects( u, nx ) ) continue;
					if ( Util.IsCycle( o.gs, u, nx ) ) continue;
					v1 = nx;
					break;
				}

				if ( v0==-1 && v1==-1 ) {
				} else if ( v0==-1 ) {
					o.gs[u] = (v1,v1);
					o.rs[v1].Add( u );
					vis[u] = visId;
					o.assigns[u] = rand.I(K);
					o.AddEs( u, v1 );
					o.AddEs( u, v1 );
					if ( N<=v1 ) {
						que.Push( v1 );
					}
				} else if ( v1==-1 ) {
					o.gs[u] = (v0,v0);
					o.rs[v0].Add( u );
					vis[u] = visId;
					o.assigns[u] = rand.I(K);
					o.AddEs( u, v0 );
					o.AddEs( u, v0 );
					if ( N<=v0 ) {
						que.Push( v0 );
					}
				} else {
					o.gs[u] = (v0,v1);
					o.rs[v0].Add( u );
					o.rs[v1].Add( u );
					vis[u] = visId;
					o.assigns[u] = rand.I(K);
					o.AddEs( u, v0 );
					o.AddEs( u, v1 );
					if ( N<=v0 ) {
						que.Push( v0 );
					}
					if ( N<=v1 ) {
						que.Push( v1 );
					}
				}
			}

			// 不正な分別器を削除
			o.RemoveInvalidSorter();

			// 処理装置をランダム配置
			for ( int i=0; i<N; i++ ) o.assigns[i] = i;
			o.assigns.Shuffle0( 0, N );

			// 未使用の分別器設置場所に無理やり配置
			if ( Prm.prmGenRootSprout ) for ( int u=N; u<NM; u++ ) {
				if ( o.assigns[u]==-1 ) {
					var path = o.GenRouteToAny( u );
					if ( path.Count!=0 ) {
						var cur = u;
						foreach ( var v in path ) {
							o.PlaceBeltWithoutCheck( cur, v, v );
							if ( N<=cur ) o.assigns[cur] = rand.I(K);
							cur = v;
						}
					}
				}
			}

			// 確率伝播と逆確率伝播を計算
			o.RebuildRs();
			o.RebuildOrder();
			o.RebuildRootLeafProbDrain();
			o.InvalidateProbs();
			o.InvalidateDrains();
			o.Eval();
			return o;
		}

		//@@ Turning -------------------------------------------------------
		public void TurningSorterBackprop( int u0 ) {
			RebuildOrder();
			visId++;
			vis[u0] = visId;
			var ls = new List<int>();
			for ( int i=orders.Count-1; N<i; i-- ) {
				var u = orders[i];
				if ( vis[u]!=visId || u<N ) continue;
				var (v0,v1)=gs[u];
				var s0 = assigns[u];
				var s1 = s0;
				var sp = 0;
				var tra = NextTraChangeSorter( u, false );
				var update = false;
				if ( tra!=null && tra.score<score-1 ) {
					s1 = tra.sorterIdx0;
					sp = tra.swap;
					assigns[u] = s1;
					update = true;
				}
				PropagateProbsDiff( u, s0, s1, sp );
				if ( sp==1 ) {
					gs[u] = (v1,v0);
				}
				if ( update ) break;
				vis[v0] = visId;
				vis[v1] = visId;
			}

			InvalidateProbs();
			InvalidateDrains();
			Eval();
			Transition.pool.Free();
		}

		public void TurningPlaceSorter() {
			for ( int i=orders.Count-1; N<i; i-- ) {
				var u = orders[i];
				if ( assigns[u]!=-1 ) continue;
				var tra = NextTraPlaceSorter( u );
				if ( (int)score-1<=(int)tra.score ) continue;
				var preScore = score;
				ApplyTra( tra );
			}
			Transition.pool.Free();
		}
		public void TurningSorter() {
			for ( int i=orders.Count-1; N<i; i-- ) {
				var u = orders[i];
				if ( assigns[u]==-1 ) continue;
				var tra = NextTraChangeSorter( u, true );
				if ( (int)score-1<=(int)tra.score ) continue;
				var preScore = score;
				ApplyTra( tra );
			}
			Transition.pool.Free();
		}
		public void TurningBelt() {
			for ( int i=orders.Count-1; N<i; i-- ) {
				var u = orders[i];
				if ( assigns[u]==-1 ) continue;
				var tra = NextTraSwitchBelt( u );
				if ( tra==null ) continue;
				if ( (int)score-1<=(int)tra.score ) continue;
				var preScore = score;
				ApplyTra( tra );
			}
			Transition.pool.Free();
		}
		public void TurningAll( double timeout, double percent ) {
			var sw = new Stopwatch();
			sw.Start();
			var nowScore = score;

			while ( true ) {
				var nowMsec = sw.ElapsedMilliseconds;
				if ( timeout < nowMsec ) break;
				var idxs = new int[orders.Count];
				for ( int i=0; i<idxs.Length; i++ ) idxs[i] = i;
				idxs.Shuffle0();

				for ( int i=orders.Count-1; N<i; i-- ) {
					nowMsec = sw.ElapsedMilliseconds;
					if ( timeout < nowMsec ) goto END;
					var u = orders[idxs[i]];
					if ( assigns[u]==-1 ) continue;
					var tra = NextTraChangeSorter( u, true );
					if ( tra==null ) continue;
					if ( (int)score<=(int)tra.score ) continue;
					var preScore = score;
					ApplyTra( tra );
				}

				if ( Prm.prmNextTraSwitchBelt ) {
					for ( int i=orders.Count-1; N<i; i-- ) {
						nowMsec = sw.ElapsedMilliseconds;
						if ( timeout < nowMsec ) goto END;
						var u = orders[i];
						if ( assigns[u]==-1 ) continue;
						var tra = NextTraSwitchBelt2( u );
						if ( tra==null ) continue;
						if ( (int)score<=(int)tra.score ) continue;
						var preScore = score;
						ApplyTra( tra );
					}
				}

				END:
				var alpha = ( nowScore - score ) / nowScore;
				if ( alpha<=percent ) break;
				nowScore = score;
				Transition.pool.Free();
			}
		}

		//@@ Print -------------------------------------------------------
		public string PrintStr() {
			return $"seed[{seed,4}] root {root,3} score {score}";
		}
		public void Print() {
			std.Print( PrintStr() );
		}

		//@@ Out -------------------------------------------------------
		public string OutputStr() {
			var sw = new StringWriter();

			sw.WriteLine( new Span<int>( assigns, 0, N ).Str() );
			sw.WriteLine( root );
			for ( int i=N; i<NM; i++ ) {
				if ( assigns[i]==-1 ) {
					sw.WriteLine( -1 );
				} else {
					sw.WriteLine( $"{assigns[i]} {gs[i].Item1} {gs[i].Item2}" );
				}
			}

			return sw.ToString();
		}
	}

	//@@ ***********************************************************************
	//@@ class Solver
	//@@ ***********************************************************************
	public class Solver {
		//@@ field -------------------------------------------------------
		public static RandomX rand = new RandomX( 42 );
		public static int N;
		public static int M;
		public static int K;
		public static int NM;

		public static Stopwatch saSw = new Stopwatch();
		public static Stopwatch debugSw = new Stopwatch();
		public static int debugCnt0 = 0;
		public static int debugCnt1 = 0;
		public static int debugCnt2 = 0;

		public static int bestScore = 0;
		public static string bestStr = String.Empty;
		public static Sample bestSam = null;

		//@@ ctor -------------------------------------------------------
		static Solver() {
		}
		public static void PreInitialize() {
			N = Terminal.N;
			M = Terminal.M;
			K = Terminal.K;
			NM = Terminal.NM;
		}
		//@@ Solve -------------------------------------------------------
		public static void Solve( Stopwatch sw ) {
			var nowTim = sw.Elapsed;

			std.Print( $"@@@@@@@@ Solver GenRoot  -> Start  {sw.Elapsed}" );
			var staSms = new List<Sample>();
			{
				for ( int i=0; i< 8; i++ ) { Sample sm = null; sm = Sample.GenRoot( 0, i  ); if ( sm!=null ) staSms.Add( sm ); }
				for ( int i=0; i<12; i++ ) { Sample sm = null; sm = Sample.GenRoot( 1, i  ); if ( sm!=null ) staSms.Add( sm ); }
				for ( int i=0; i<12; i++ ) { Sample sm = null; sm = Sample.GenRoot( 2, i  ); if ( sm!=null ) staSms.Add( sm ); }
//				for ( int i=0; i< 1; i++ ) { Sample sm = null; sm = Sample.GenRoot( 3, 32 ); if ( sm!=null ) staSms.Add( sm ); }
				foreach( var sam in staSms ) {
//					sam.Print();
				}
			}

			std.Print( $"@@@@@@@@ Solver Pre0     -> Start  {sw.Elapsed}" );
			{
				// ゴミの流れが安定するまでルート変更はしない
				Prm.prmNextTraMergeBelt     = false;
				Prm.prmNextTraSwitchBelt    = false;
				Prm.prmNextTraAcceptSelf    = false;
				Prm.prmNextTraSwapProc      = false;
				Prm.prmNextTraPlaceSorter   = false;

				staSms = SimulatedAnnealing.RunHC( timeout:300, staSms:staSms, inplace:true );

				staSms = staSms.OrderBy(i=>i.score).ToList();
				foreach( var sam in staSms ) {
					sam.Print();
				}
			}

			std.Print( $"@@@@@@@@ Solver Pre1     -> Start  {sw.Elapsed}" );
			{
				staSms = staSms.Take(8).ToList();
				foreach( var sam in staSms ) {
					sam.RebuildAll();
					sam.Eval();
				}

				// ゴミの流れが安定してきたらルートをいじって選別する
				Prm.prmNextTraMergeBelt     = true;
				Prm.prmNextTraSwitchBelt    = true;
				Prm.prmNextTraAcceptSelf    = true;
				Prm.prmNextTraSwapProc      = false;
				Prm.prmNextTraPlaceSorter   = true;
				Prm.prmNextTraSwitchMaxNear = (int)Prm.InterpolateXY( M, 50, 64, 1000, 32 );

				staSms = SimulatedAnnealing.RunHC( timeout:400, staSms:staSms, inplace:true );

				staSms = staSms.OrderBy(i=>i.score).ToList();
				foreach( var sam in staSms ) {
					sam.Print();
				}
			}

			std.Print( $"@@@@@@@@ Solver Post     -> Start  {sw.Elapsed}" );
			{
				staSms = staSms.Take(1).ToList();
				foreach( var sam in staSms ) {
					sam.RebuildAll();
					sam.Eval();
				}

				// 選別したサンプルでSA
				var timeout = 1800 - sw.ElapsedMilliseconds;
				foreach( var sam in staSms ) {
					sam.RebuildAll();
					sam.Eval();
				}
				staSms = SimulatedAnnealing.RunMI( timeout:timeout, staSms:staSms );

				staSms = staSms.OrderBy(i=>i.score).ToList();
				foreach( var sam in staSms ) {
					sam.Print();
				}
			}

			std.Print( $"@@@@@@@@ Solver Finalize -> Start  {sw.Elapsed}" );
			{
				staSms = staSms.Take(1).ToList();
				foreach( var sam in staSms ) {
					sam.RebuildAll();
					sam.Eval();
				}

				//最後に全分岐器をチューニング
				Prm.prmNextTraMergeBelt     = true;
				Prm.prmNextTraSwitchBelt    = true;
				Prm.prmNextTraAcceptSelf    = true;
				Prm.prmNextTraSwapProc      = false;
				Prm.prmNextTraPlaceSorter   = true;
				var tuningTimeout = 1900 - sw.ElapsedMilliseconds;
				foreach( var sam in staSms ) {
					var preScore = sam.score;
					sam.TurningAll( timeout:tuningTimeout, percent:0.0 );
					std.Print( $"Improve {100.0*(preScore-sam.score)/preScore:F}% {preScore} -> {sam.score}" );
				}
			}

			std.Print( $"@@@@@@@@ Solver Output   -> Start  {sw.Elapsed}" );
			staSms = staSms.OrderBy(i=>i.score).Take(1).ToList();
			var finalSam = staSms[0];

			//念の為に再計算
//			finalSam.RemoveInvalidSorter();
//			finalSam.RemoveNoUseSorter();
			finalSam.RebuildAll();
			finalSam.Eval();
			finalSam.Print();

			bestStr = finalSam.OutputStr();
			bestScore = (int)finalSam.score;

			Transition.pool.Print();
			Sample.pool.Print();

			std.Print( $"ScoreEst {Terminal.EstScore}" );
			std.Print( $"Score    {bestScore}" );

			std.Print( $"debug tim {debugSw.Elapsed} cnt0 {Solver.debugCnt0} cnt1 {Solver.debugCnt1} cnt2 {Solver.debugCnt2}" );
			std.Print( $"Solver.Solve > Tim {sw.Elapsed} -> Finish" );
			var curProcess = Process.GetCurrentProcess();
			std.Print( $"PeakWorkingSet {curProcess.PeakWorkingSet64/1000} KBytes" );
			std.Print( $" NowWorkingSet {curProcess.WorkingSet64/1000} KBytes" );
		}

		//@@ Print -------------------------------------------------------
		public static void Print() {
		}
	}

	//@@ **********************************************************************
	//@@ class App
	//@@ **********************************************************************
	public static class App {

		// Run ----------------------------------------------------------------
		public static void Run( string[] args, CommandOpt2 opt, Stopwatch sw ) {
			sw.Start();

			//@@ パラメータ **************************************
			std.isPrint = Prm.isPrint;
			std.isInfo  = Prm.isInfo;
			std.isError = Prm.isError;

			//@@ 初期化 **************************************
			var rand = new RandomX( 42 );

//			var input = Input.LoadStd();
//			var input = Input.LoadFile( "in/0000.txt" );
//			var input = Input.LoadGen( N:20, M:1000, K:80 );
			var input = Prm.isFis ? Input.LoadGen( N:20, M:1000, K:80 ) : Input.LoadStd();

			Terminal.PreInitialize( input );
			Sample.PreInitialize();
			Solver.PreInitialize();

//			Sample.pool.PreInitialize( 4*1024 );
//			Transition.pool.PreInitialize( 4*1024 );

			Terminal.Print( isDetail:false );

			//@@ solve *******************************************************
			Solver.Solve( sw );
			var ans = Solver.bestScore;

			//@@ finish *******************************************************
			if ( Prm.isSim ) {
				if ( Prm.isOptuna ) {
					Console.WriteLine( $"{ans}" );
				} else if ( Prm.isTim ) {
					Console.Error.WriteLine( $"{sw.Elapsed}" );
				} else {
//					Solver.bestPars.WriteErr();
					Console.Error.WriteLine( $"{ans}" );
//					Console.Error.WriteLine( $"{Solver.bestSam.name}" );
//					Console.Error.WriteLine( $"{Terminal.K}" );
//					Console.Error.WriteLine( $"{ret}" );
//					Console.Error.WriteLine( $"{sw.Elapsed}" );
				}

//				File.WriteAllLines( @"r:/out.txt", Terminal.logs );
				std.Info( $"{sw.Elapsed}" );
#if TEST
				if ( Prm.isClip ) {
					if ( Solver.bestStr!=String.Empty ) {
						Clipboard.SetText( String.Join( "", Solver.bestStr ) );
					}
				} else {
//					Out.WriteLine( Solver.bestStr );
				}
#endif

			} else {
				Out.WriteLine( Solver.bestStr );
//				Out.WriteLine( ans );
			}
		}

		// Main ----------------------------------------------------------------
		[STAThreadAttribute]
		public static void Main( string[] args ) {
			var sw = new Stopwatch();
			sw.Start();
//			var writer = new StreamWriter( Console.OpenStandardOutput() ){ AutoFlush=false };
//			Console.SetOut( writer );

			try {
				var cmds = new CommandOpt2( args );
				if ( Prm.Parse( cmds.GetOptArgs() ) is var r && (!r.Item1) ) {
					Console.WriteLine( r.Item2 );
					return;
				}

				var sw0 = new Stopwatch();
				Run( args, cmds, sw0 );
				std.Info( $"TestTime {sw0.Elapsed}" );
			} catch ( Exception e ) {
//				Console.WriteLine( e );
				Console.Error.WriteLine( e );
			}

//			var curProcess = Process.GetCurrentProcess();
//			std.Info( $"MaxWarkingSet {curProcess.PeakWorkingSet64/1000} KBytes" );
//			std.Info( $"NowWarkingSet {curProcess.WorkingSet64/1000} KBytes" );

			sw.Stop();
			std.Info( $"MainTime {sw.Elapsed}" );
			Console.Out.Flush();
		}
	}

}





