fn main() {
    ahc008::algorithms::solve();
}

#[macro_use]
mod macros {
    #[macro_export]
    macro_rules! chmin {
        ($base:expr, $($cmps:expr),+ $(,)*) => {{
            let cmp_min = min!($($cmps),+);
            if $base > cmp_min {
                $base = cmp_min;
                true
            } else {
                false
            }
        }};
    }

    #[macro_export]
    macro_rules! chmax {
        ($base:expr, $($cmps:expr),+ $(,)*) => {{
            let cmp_max = max!($($cmps),+);
            if $base < cmp_max {
                $base = cmp_max;
                true
            } else {
                false
            }
        }};
    }

    #[macro_export]
    macro_rules! min {
        ($a:expr $(,)*) => {{
            $a
        }};
        ($a:expr, $b:expr $(,)*) => {{
            std::cmp::min($a, $b)
        }};
        ($a:expr, $($rest:expr),+ $(,)*) => {{
            std::cmp::min($a, min!($($rest),+))
        }};
    }

    #[macro_export]
    macro_rules! max {
        ($a:expr $(,)*) => {{
            $a
        }};
        ($a:expr, $b:expr $(,)*) => {{
            std::cmp::max($a, $b)
        }};
        ($a:expr, $($rest:expr),+ $(,)*) => {{
            std::cmp::max($a, max!($($rest),+))
        }};
    }

    #[macro_export]
    macro_rules! mat {
        ($e:expr; $d:expr) => { vec![$e; $d] };
        ($e:expr; $d:expr $(; $ds:expr)+) => { vec![mat![$e $(; $ds)*]; $d] };
    }

    #[macro_export]
    macro_rules! watch_us {
        ($st:stmt) => {
            {
                let since = std::time::Instant::now();
                $st
                let until = std::time::Instant::now();
                let elapsed = (until - since).as_micros();
                elapsed
            }
        };
        ($st:stmt;) => {
            watch_us!($st)
        };
    }
}

mod ahc008 {
    use std::time::Instant;

    use crate::grid;
    use crate::grid::{Coordinate, CoordinateDiff, ADJACENTS};
    #[allow(unused_imports)]
    use rand::prelude::*;

    const MAP_SIZE: usize = 30 + 2;
    const MAX_TURN: usize = 300;

    type PlacementAssign = (Coordinate, u8);

    #[derive(Debug, Clone)]
    struct Input {
        pet_count: usize,
        pets: Vec<Pet>,
        people_count: usize,
        people: Vec<Coordinate>,
        barricades: Vec<Coordinate>,
        since: Instant,
    }

    impl Input {
        fn get_asset_map(&self) -> Vec<Vec<Vec<Asset>>> {
            let mut map = mat![vec![]; MAP_SIZE; MAP_SIZE];

            for (i, pet) in self.pets.iter().enumerate() {
                let coordinate = pet.coordinate;
                let pet_type = pet.pet_type;
                let asset = Asset::Pet(i, pet_type);
                map[coordinate.row][coordinate.col].push(asset);
            }

            for (i, person) in self.people.iter().enumerate() {
                let asset = Asset::Person(i);
                map[person.row][person.col].push(asset);
            }

            for barricade in self.barricades.iter() {
                let asset = Asset::Barricade;
                map[barricade.row][barricade.col].push(asset);
            }

            map
        }

        fn get_placeable_map(&self) -> Vec<Vec<bool>> {
            let mut map = mat![true; MAP_SIZE; MAP_SIZE];

            for pet in self.pets.iter() {
                let coordinate = pet.coordinate;
                map[coordinate.row][coordinate.col] = false;

                for &adj in ADJACENTS.iter() {
                    let next = coordinate + adj;
                    map[next.row][next.col] = false;
                }
            }

            for person in self.people.iter() {
                map[person.row][person.col] = false;
            }

            // 不正な解ではないが、バグの温床になるためNGにしておく
            for barricade in self.barricades.iter() {
                map[barricade.row][barricade.col] = false;
            }

            map
        }

        fn get_barricade_map(&self) -> Vec<Vec<bool>> {
            let mut map = mat![false; MAP_SIZE; MAP_SIZE];

            for barricade in self.barricades.iter() {
                map[barricade.row][barricade.col] = true;
            }

            map
        }

        fn update_people(&mut self, commands: &[Command]) {
            let placeable = self.get_placeable_map();
            let barricades = self.get_barricade_map();

            for (person, &command) in self.people.iter_mut().zip(commands) {
                match command {
                    Command::None => (),
                    Command::Put(diff) => {
                        let next = *person + diff;
                        debug_assert!(placeable[next.row][next.col]);
                        self.barricades.push(next);
                    }
                    Command::Move(diff) => {
                        let next = *person + diff;
                        debug_assert!(!barricades[next.row][next.col]);
                        *person = next;
                    }
                }
            }
        }

        fn update_pets(&mut self, commands: &[Vec<Command>]) {
            let barricades = self.get_barricade_map();

            for (pet, commands) in self.pets.iter_mut().zip(commands.iter()) {
                for &command in commands.iter() {
                    match command {
                        Command::Move(diff) => {
                            let next = pet.coordinate + diff;
                            debug_assert!(!barricades[next.row][next.col]);
                            pet.coordinate = next;
                        }
                        _ => unreachable!(),
                    }
                }
            }
        }
    }

    impl std::fmt::Display for Input {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            for row in 0..MAP_SIZE {
                let map = self.get_asset_map();

                for col in 0..MAP_SIZE {
                    if map[row][col].len() == 0 {
                        let result = write!(f, " ");

                        if result.is_err() {
                            return result;
                        }
                    } else {
                        let &asset = map[row][col].last().unwrap();

                        let result = match asset {
                            Asset::Person(_) => write!(f, "P"),
                            Asset::Pet(_, _) => write!(f, "n"),
                            Asset::Barricade => write!(f, "#"),
                        };

                        if result.is_err() {
                            return result;
                        }
                    }
                }

                let result = writeln!(f, "");

                if result.is_err() {
                    return result;
                }
            }

            Ok(())
        }
    }

    #[derive(Debug, Clone, Copy)]
    struct Pet {
        coordinate: Coordinate,
        pet_type: PetType,
    }

    impl Pet {
        fn new(coordinate: Coordinate, pet_type: PetType) -> Self {
            Self {
                coordinate,
                pet_type,
            }
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    enum PetType {
        Cow,
        Pig,
        Rabbit,
        Dog,
        Cat,
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    enum Command {
        None,
        Put(CoordinateDiff),
        Move(CoordinateDiff),
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    enum Asset {
        Person(usize),
        Pet(usize, PetType),
        Barricade,
    }

    mod configs {
        use super::{neural_network, Input, MAP_SIZE};
        use std::env;

        #[derive(Debug, Clone)]
        pub(super) struct Config {
            pub(super) aisle_rows: Vec<usize>,
            pub(super) aisle_cols: Vec<usize>,
            pub(super) map: Vec<Vec<u8>>,
        }

        impl Config {
            pub(super) fn new(
                aisle_rows: Vec<usize>,
                aisle_cols: Vec<usize>,
                map: &[&[u8; MAP_SIZE]],
            ) -> Self {
                let map = map
                    .iter()
                    .map(|line| line.iter().map(|&c| c).collect())
                    .collect();

                Self {
                    aisle_rows,
                    aisle_cols,
                    map,
                }
            }
        }

        #[allow(dead_code)]
        const MAP_CANDIDATES: [[&[u8; MAP_SIZE]; MAP_SIZE]; 5] = [
            [
                // 2列エ型
                // 1   5    10   15   20   25   30
                b"################################", // 0
                b"#......X.|.X...X....X.|.X......#",
                b"#~~~~~~~)|(~~~~~X~~~~)|(~~~~~~~#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#-------)|<---------->|(-------#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#", // 5
                b"#~~~~~~~)|(~~~~~X~~~~)|(~~~~~~~#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#-------)|<---------->|(-------#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#~~~~~~~)|(~~~~~X~~~~)|(~~~~~~~#", // 10
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#-------)|<---------->|(-------#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#~~~~~~~)|(~~~~~X~~~~)|(~~~~~~~#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#", // 15
                b"#-------)|<---------->|(-------#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#~~~~~~~)|(~~~~~X~~~~)|(~~~~~~~#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#-------)|<---------->|(-------#", // 20
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#~~~~~~~)|(~~~~~X~~~~)|(~~~~~~~#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#-------)|<---------->|(-------#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#", // 25
                b"#~~~~~~~)|(~~~~~X~~~~)|(~~~~~~~#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#-------)|<---------->|(-------#",
                b"#XXXXXXX.|.XXXXX.XXXX.|.XXXXXXX#",
                b"#~~~~~~~)|(~~~~~X~~~~)|(~~~~~~~#", // 30
                b"################################",
            ],
            [
                // 3列エ型
                // 1   5    10   15   20   25   30
                b"################################", // 0
                b"#..X.|.X...X.X.|.X..X..X.|.X...#",
                b"#~~~)|(~~~X~~~)|(~~~X~~~)|(~~~~#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#---)|<------->|<------->|(----#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#", // 5
                b"#~~~)|(~~~X~~~)|(~~~X~~~)|(~~~~#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#---)|<------->|<------->|(----#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#~~~)|(~~~X~~~)|(~~~X~~~)|(~~~~#", // 10
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#---)|<------->|<------->|(----#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#~~~)|(~~~X~~~)|(~~~X~~~)|(~~~~#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#", // 15
                b"#---)|<------->|<------->|(----#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#~~~)|(~~~X~~~)|(~~~X~~~)|(~~~~#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#---)|<------->|<------->|(----#", // 20
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#~~~)|(~~~X~~~)|(~~~X~~~)|(~~~~#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#---)|<------->|<------->|(----#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#", // 25
                b"#~~~)|(~~~X~~~)|(~~~X~~~)|(~~~~#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#---)|<------->|<------->|(----#",
                b"#XXX.|.XXX.XXX.|.XXX.XXX.|.XXXX#",
                b"#~~~)|(~~~X~~~)|(~~~X~~~)|(~~~~#", // 30
                b"################################",
            ],
            [
                // 4列ニ型
                // 1   5    10   15   20   25   30
                b"################################", // 0
                b"#..X.|.X..X.|.X..X.|.X..X.|.X..#",
                b"#~~~)|<~~~~>|<~~~~>|<~~~~>|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#", // 5
                b"#~~~)|<~~~~>|<~~~~>|<~~~~>|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#~~~)|<~~~~>|<~~~~>|<~~~~>|(~~~#", // 10
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#~~~)|<~~~~>|<~~~~>|<~~~~>|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#", // 15
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#~~~)|<~~~~>|<~~~~>|<~~~~>|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#", // 20
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#~~~)|<~~~~>|<~~~~>|<~~~~>|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#", // 25
                b"#~~~)|<~~~~>|<~~~~>|<~~~~>|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#~~~)|<~~~~>|<~~~~>|<~~~~>|(~~~#", // 30
                b"################################",
            ],
            [
                // 5列ニ型
                // 1   5    10   15   20   25   30
                b"################################", // 0
                b"#|.X.X.|.X..X.|.X.X.|.X..X.|.X.#",
                b"#|<~~~>|<~~~~>|<~~~>|<~~~~>|(~~#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<--->|<---->|<--->|<---->|(--#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#", // 5
                b"#|<~~~>|<~~~~>|<~~~>|<~~~~>|(~~#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<--->|<---->|<--->|<---->|(--#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<~~~>|<~~~~>|<~~~>|<~~~~>|(~~#", // 10
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<--->|<---->|<--->|<---->|(--#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<~~~>|<~~~~>|<~~~>|<~~~~>|(~~#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#", // 15
                b"#|<--->|<---->|<--->|<---->|(--#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<~~~>|<~~~~>|<~~~>|<~~~~>|(~~#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<--->|<---->|<--->|<---->|(--#", // 20
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<~~~>|<~~~~>|<~~~>|<~~~~>|(~~#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<--->|<---->|<--->|<---->|(--#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#", // 25
                b"#|<~~~>|<~~~~>|<~~~>|<~~~~>|(~~#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<--->|<---->|<--->|<---->|(--#",
                b"#|.XXX.|.XXXX.|.XXX.|.XXXX.|.XX#",
                b"#|<~~~>|<~~~~>|<~~~>|<~~~~>|(~~#", // 30
                b"################################",
            ],
            [
                // 4列コ型
                // 1   5    10   15   20   25   30
                b"################################", // 0
                b"#..X.|.X..X.|.X..X.|.X..X.|.X..#",
                b"#~~~)|(~~~~X|X~~~~)|(~~~~X|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#", // 5
                b"#~~~)|X~~~~)|(~~~~X|X~~~~)|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#~~~)|(~~~~X|X~~~~)|(~~~~X|(~~~#", // 10
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#~~~)|X~~~~)|(~~~~X|X~~~~)|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#", // 15
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#~~~)|(~~~~X|X~~~~)|(~~~~X|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#", // 20
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#~~~)|X~~~~)|(~~~~X|X~~~~)|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#", // 25
                b"#~~~)|(~~~~X|X~~~~)|(~~~~X|(~~~#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#---)|<---->|<---->|<---->|(---#",
                b"#XXX.|.XXXX.|.XXXX.|.XXXX.|.XXX#",
                b"#~~~)|X~~~~)|(~~~~X|X~~~~)|(~~~#", // 30
                b"################################",
            ],
        ];

        pub(super) fn get_config_list() -> Vec<Config> {
            let aisle_rows = vec![2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30];
            let aisle_cols0 = vec![9, 22];
            let aisle_cols1 = vec![5, 15, 25];
            let aisle_cols2 = vec![5, 12, 19, 26];
            let aisle_cols3 = vec![1, 7, 14, 20, 27];

            vec![
                Config::new(aisle_rows.clone(), aisle_cols0.clone(), &MAP_CANDIDATES[0]),
                Config::new(aisle_rows.clone(), aisle_cols1.clone(), &MAP_CANDIDATES[1]),
                Config::new(aisle_rows.clone(), aisle_cols2.clone(), &MAP_CANDIDATES[2]),
                Config::new(aisle_rows.clone(), aisle_cols3.clone(), &MAP_CANDIDATES[3]),
                Config::new(aisle_rows.clone(), aisle_cols2.clone(), &MAP_CANDIDATES[4]),
            ]
        }

        pub(super) fn get_config_index(input: &Input) -> usize {
            let args = env::args().collect::<Vec<_>>();

            if args.len() >= 2 {
                args[1].parse().unwrap()
            } else {
                choose_config_index_by_nn(input)
            }
        }

        fn choose_config_index_by_nn(input: &Input) -> usize {
            let x = normalize_input(input);
            let model = neural_network::generate_model();
            let predicted_scores = model.predict(&x);

            let mut best_index = 0;
            let mut best_score = 0.0;

            for (i, &score) in predicted_scores.iter().enumerate() {
                eprintln!("config{} pred: {}", i, score);
                if chmax!(best_score, score) {
                    best_index = i;
                }
            }

            best_index
        }

        fn normalize_input(input: &Input) -> Vec<f64> {
            let mut pet_counts = vec![0; 5];

            for pet in input.pets.iter() {
                let index = match pet.pet_type {
                    super::PetType::Cow => 0,
                    super::PetType::Pig => 1,
                    super::PetType::Rabbit => 2,
                    super::PetType::Dog => 3,
                    super::PetType::Cat => 4,
                };
                pet_counts[index] += 1;
            }

            let people = normalize_people(input.people_count);
            let pets = normalize_all_pets(input.pet_count);
            let cows = normalize_each_pet(pet_counts[0]);
            let pigs = normalize_each_pet(pet_counts[1]);
            let rabbits = normalize_each_pet(pet_counts[2]);
            let dogs = normalize_each_pet(pet_counts[3]);
            let cats = normalize_each_pet(pet_counts[4]);

            vec![people, pets, cows, pigs, rabbits, dogs, cats]
        }

        fn normalize_people(people_count: usize) -> f64 {
            (people_count as f64 - 5.0) / 5.0
        }

        fn normalize_all_pets(pet_count: usize) -> f64 {
            (pet_count as f64 - 10.0) / 10.0
        }

        fn normalize_each_pet(pet_count: usize) -> f64 {
            pet_count as f64 / 10.0
        }
    }

    pub(crate) mod algorithms {
        use std::collections::VecDeque;

        use rand_pcg::Pcg64Mcg;

        use crate::ahc008::io::{Judge, VirtualJudge};

        use super::*;

        pub(crate) fn solve() {
            const DICISION_TURN: usize = 275;
            let mut input = io::initialize();
            let config_list = configs::get_config_list();
            let config = &config_list[configs::get_config_index(&input)];
            let mut barricade_assigns = preprocess::calculate_placement_plan(&input, &config);
            let mut judge = io::RealJudge::new();
            let mut chasing_mode = false;
            let mut rng = Pcg64Mcg::new(42);

            for turn in 0..MAX_TURN {
                eprintln!("turn: {}", turn);
                let mut people_commands = vec![None; input.people_count];
                let mut next_placeable_map = input.get_placeable_map();
                let mut next_barricade_map = input.get_barricade_map();

                if turn == DICISION_TURN {
                    chasing_mode =
                        decide_chasing_or_not(&input, turn, config, &mut barricade_assigns);
                }

                if !chasing_mode {
                    run_normal_mode(
                        &input,
                        config,
                        &mut people_commands,
                        &mut barricade_assigns,
                        &mut next_placeable_map,
                        &mut next_barricade_map,
                        &mut rng,
                    );
                } else {
                    run_chasing_mode(
                        &input,
                        config,
                        turn,
                        &mut people_commands,
                        &mut next_placeable_map,
                        &mut next_barricade_map,
                    );
                }

                let people_commands = clean_up_commands(&input, &people_commands);

                input.update_people(&people_commands);
                judge.write_commands(&people_commands);
                let pet_commands = judge.read_commands(&input);
                input.update_pets(&pet_commands);
            }
        }

        fn run_normal_mode(
            input: &Input,
            config: &configs::Config,
            people_commands: &mut Vec<Option<Command>>,
            barricade_assigns: &mut Vec<std::collections::VecDeque<(Coordinate, u8)>>,
            next_placeable_map: &mut Vec<Vec<bool>>,
            next_barricade_map: &mut Vec<Vec<bool>>,
            rng: &mut Pcg64Mcg,
        ) {
            first_phase::get_commands(
                input,
                people_commands,
                barricade_assigns,
                next_placeable_map,
                next_barricade_map,
            );
            second_phase::get_commands(
                input,
                &config,
                people_commands,
                barricade_assigns,
                next_barricade_map,
            );
            third_phase::get_commands(
                input,
                &config,
                next_barricade_map,
                next_placeable_map,
                people_commands,
                rng,
            );
            fourth_phase::get_commands(input, &config, people_commands);
        }

        fn run_chasing_mode(
            input: &Input,
            config: &configs::Config,
            turn: usize,
            people_commands: &mut Vec<Option<Command>>,
            next_placeable_map: &mut Vec<Vec<bool>>,
            next_barricade_map: &mut Vec<Vec<bool>>,
        ) {
            fifth_phase::get_commands(
                input,
                &config,
                turn,
                people_commands,
                next_placeable_map,
                next_barricade_map,
            );
        }

        fn clean_up_commands(input: &Input, commands: &[Option<Command>]) -> Vec<Command> {
            let mut next_commands = vec![Command::None; input.people_count];
            let mut next_placeable_map = input.get_placeable_map();

            for (i, (&command, &person)) in commands.iter().zip(input.people.iter()).enumerate() {
                if let Some(Command::Move(adj)) = command {
                    let next = person + adj;
                    next_placeable_map[next.row][next.col] = false;
                    next_commands[i] = Command::Move(adj);
                }
            }

            // 轢かないように確認
            // できれば引っかかりたくない
            for (i, (&command, &person)) in commands.iter().zip(input.people.iter()).enumerate() {
                if let Some(Command::Put(adj)) = command {
                    let next = person + adj;
                    if next_placeable_map[next.row][next.col] {
                        next_commands[i] = Command::Put(adj);
                    } else {
                        eprintln!(
                            "[WARNING!] tried to put invaild barricade at ({}, {})",
                            next.row, next.col
                        );
                    }
                }
            }

            next_commands
        }

        fn decide_chasing_or_not(
            input: &Input,
            turn: usize,
            config: &configs::Config,
            barricade_assigns: &mut Vec<VecDeque<(Coordinate, u8)>>,
        ) -> bool {
            const TIME_LIMIT: u128 = 2700;
            const SEED_INCREMENT: u64 = 998244353;
            let (connection_map, group_count) = common::get_connection_map(input);
            let majority_group =
                common::get_majority_people_group(input, &connection_map, group_count);
            let pet_count =
                common::get_alive_pet_indices(input, &connection_map, majority_group).len();
            if pet_count == 0 {
                return false;
            }

            let mut total_normal_score = 0;
            let mut total_chasing_score = 0;
            let mut iter = 0;

            while (Instant::now() - input.since).as_millis() < TIME_LIMIT {
                iter += 1;
                let seed = SEED_INCREMENT * iter;
                let normal_score =
                    try_normal_mode(input.clone(), config, turn, seed, barricade_assigns);
                let chasing_score = try_chasing_mode(input.clone(), config, turn, seed);

                total_normal_score += normal_score;
                total_chasing_score += chasing_score;

                eprintln!("=== iter {} ===", iter);
                eprintln!("normal: {}", normal_score);
                eprintln!("chasing: {}", chasing_score);
            }

            eprintln!("=== total ===");
            eprintln!("normal: {}", total_normal_score);
            eprintln!("chasing: {}", total_chasing_score);

            total_normal_score < total_chasing_score
        }

        fn try_normal_mode(
            mut input: Input,
            config: &configs::Config,
            turn: usize,
            seed: u64,
            barricade_assigns: &mut Vec<VecDeque<(Coordinate, u8)>>,
        ) -> i64 {
            let mut judge = VirtualJudge::new(&input, turn, seed);
            let mut rng = Pcg64Mcg::new(42);

            for _turn in turn..MAX_TURN {
                let mut people_commands = vec![None; input.people_count];
                let mut next_placeable_map = input.get_placeable_map();
                let mut next_barricade_map = input.get_barricade_map();

                run_normal_mode(
                    &input,
                    config,
                    &mut people_commands,
                    barricade_assigns,
                    &mut next_placeable_map,
                    &mut next_barricade_map,
                    &mut rng,
                );

                let people_commands = clean_up_commands(&input, &people_commands);

                input.update_people(&people_commands);
                judge.write_commands(&people_commands);
                let pet_commands = judge.read_commands(&input);
                input.update_pets(&pet_commands);
            }

            judge.calculate_score(&input) as i64
        }

        fn try_chasing_mode(
            mut input: Input,
            config: &configs::Config,
            turn: usize,
            seed: u64,
        ) -> i64 {
            let mut judge = VirtualJudge::new(&input, turn, seed);

            for _turn in turn..MAX_TURN {
                let mut people_commands = vec![None; input.people_count];
                let mut next_placeable_map = input.get_placeable_map();
                let mut next_barricade_map = input.get_barricade_map();

                run_chasing_mode(
                    &input,
                    config,
                    turn,
                    &mut people_commands,
                    &mut next_placeable_map,
                    &mut next_barricade_map,
                );

                let people_commands = clean_up_commands(&input, &people_commands);

                input.update_people(&people_commands);
                judge.write_commands(&people_commands);
                let pet_commands = judge.read_commands(&input);
                input.update_pets(&pet_commands);
            }

            judge.calculate_score(&input) as i64
        }

        /// 初期バリケードの配置分担をDPで決定する
        mod preprocess {
            use std::collections::VecDeque;

            use crate::{
                ahc008::configs::Config,
                ahc008::{Input, PlacementAssign, MAP_SIZE},
                grid::{Coordinate, CoordinateDiff, ADJACENTS},
            };

            pub(super) fn calculate_placement_plan(
                input: &Input,
                config: &Config,
            ) -> Vec<VecDeque<PlacementAssign>> {
                let path0 = generate_path(config, true);
                let (assigns0, dist0) = assign_paths(input, config, &path0);

                let path1 = generate_path(config, false);
                let (assigns1, dist1) = assign_paths(input, config, &path1);

                if dist0 <= dist1 {
                    assigns0
                } else {
                    assigns1
                }
            }

            fn generate_path(config: &Config, mut left_to_right: bool) -> Vec<Coordinate> {
                let mut path = vec![];
                let ascending = (1..(MAP_SIZE - 1)).collect::<Vec<_>>();
                let descending = (1..(MAP_SIZE - 1)).rev().collect::<Vec<_>>();

                for &row in config.aisle_rows.iter() {
                    if config.map[row][MAP_SIZE - 2] != b'~' {
                        continue;
                    }

                    let cols = if left_to_right {
                        &ascending
                    } else {
                        &descending
                    };

                    left_to_right ^= true;

                    for &col in cols.iter() {
                        let map = &config.map;
                        if map[row][col] == b'X'
                            || map[row - 1][col] == b'X'
                            || map[row + 1][col] == b'X'
                        {
                            path.push(Coordinate::new(row, col));
                        }
                    }
                }

                path
            }

            fn assign_paths(
                input: &Input,
                config: &Config,
                path: &[Coordinate],
            ) -> (Vec<VecDeque<PlacementAssign>>, i32) {
                let prefix_dists = get_prefix_dists(path);
                let prefix_placements = get_prefix_placements(config, path);

                // dpの遷移先の幅
                let window_size = path.len() * 2 / input.people_count;
                let flag_max = 1 << input.people_count;
                let mut dp = mat![std::i32::MAX / 2; flag_max; path.len() + 1];
                let mut dp_from = mat![(0, 0, false); flag_max; path.len() + 1];
                dp[0][0] = 0;

                for flag in 0..(flag_max - 1) {
                    for from in 0..path.len() {
                        for to in (from + 1)..=(path.len().min(from + window_size)) {
                            for (i, &person) in input.people.iter().enumerate() {
                                let person_flag = 1 << i;
                                if (flag & person_flag) > 0 {
                                    continue;
                                }

                                let dist = prefix_dists[to - 1] - prefix_dists[from];
                                let placement = prefix_placements[to] - prefix_placements[from];

                                // 順方向の移動
                                let cost = person.distance_to(path[from]) + dist + placement;

                                let cost_max = dp[flag][from].max(cost);
                                if chmin!(dp[flag | person_flag][to], cost_max) {
                                    dp_from[flag | person_flag][to] = (flag, from, true);
                                }

                                // 逆方向の移動
                                let cost = person.distance_to(path[to - 1]) + dist + placement;
                                let cost_max = dp[flag][from].max(cost);
                                if chmin!(dp[flag | person_flag][to], cost_max) {
                                    dp_from[flag | person_flag][to] = (flag, from, false);
                                }
                            }
                        }
                    }
                }

                let mut flag = flag_max - 1;
                let mut i = path.len();
                let best_dist = dp[flag][i];

                let mut assigns = vec![VecDeque::new(); input.people_count];

                while flag > 0 {
                    let (next_flag, next_i, direction) = dp_from[flag][i];
                    let person = (flag ^ next_flag).trailing_zeros() as usize;
                    let assign = &mut assigns[person];

                    if direction {
                        let path = path[next_i..i].iter().map(|&c| c).collect::<Vec<_>>();
                        *assign = assign_single_path(config, &path);
                    } else {
                        let path = path[next_i..i].iter().rev().map(|&c| c).collect::<Vec<_>>();
                        *assign = assign_single_path(config, &path);
                    }

                    flag = next_flag;
                    i = next_i;
                }

                (assigns, best_dist)
            }

            fn assign_single_path(
                config: &Config,
                path: &[Coordinate],
            ) -> VecDeque<PlacementAssign> {
                let mut assign = VecDeque::new();

                if path.len() == 0 {
                    return assign;
                }

                let map = &config.map;
                let mut current = path[0];
                let mut index = 0;

                loop {
                    if config.aisle_rows.binary_search(&current.row).is_ok() {
                        // 置く必要があるかどうか調べる
                        let mut flag = 0u8;

                        for (i, &adj) in ADJACENTS.iter().enumerate() {
                            let next = current + adj;

                            if map[next.row][next.col] == b'X' {
                                // 横方向の場合は他人のものを奪っちゃダメ
                                if adj.dc != 0 && path.iter().all(|&c| c != next) {
                                    continue;
                                }

                                flag |= 1 << i;

                                for (prev_c, prev_i) in assign.iter_mut() {
                                    for (dir, &adj) in ADJACENTS.iter().enumerate() {
                                        let prev_nx = *prev_c + adj;

                                        // オフにする
                                        if next == prev_nx {
                                            *prev_i &= !(1 << dir);
                                        }
                                    }
                                }
                            }
                        }

                        // 配置場所として登録
                        if flag > 0 {
                            assign.push_back((current, flag));
                        }
                    }

                    // 目的地に着いたか？
                    if current == path[index] {
                        index += 1;

                        if index == path.len() {
                            break;
                        }
                    }

                    // 移動
                    let target = path[index];

                    if current.row < target.row {
                        current.row += 1;
                    } else if current.row > target.row {
                        current.row -= 1;
                    } else if current.col < target.col {
                        current.col += 1;
                    } else if current.col > target.col {
                        current.col -= 1;
                    }
                }

                assign
            }

            fn get_prefix_dists(path: &[Coordinate]) -> Vec<i32> {
                let mut distances = vec![0];

                for i in 1..path.len() {
                    let dist = path[i].distance_to(path[i - 1]);
                    distances.push(dist + distances[i - 1]);
                }

                distances
            }

            fn get_prefix_placements(config: &Config, path: &[Coordinate]) -> Vec<i32> {
                let mut placements = vec![0];
                let map = &config.map;
                let adjacents = vec![
                    CoordinateDiff::new(!0, 0),
                    CoordinateDiff::new(0, 0),
                    CoordinateDiff::new(1, 0),
                ];

                for i in 0..path.len() {
                    let mut count = 0;

                    for &adj in adjacents.iter() {
                        let next = path[i] + adj;
                        if map[next.row][next.col] == b'X' {
                            count += 1;
                        }
                    }

                    placements.push(count + placements[i]);
                }

                placements
            }
        }

        /// 初期バリケードの配置処理を行う
        mod first_phase {
            use std::collections::VecDeque;

            use crate::{
                ahc008::{algorithms::common, Command, Input, PlacementAssign},
                grid::ADJACENTS,
            };

            pub(super) fn get_commands(
                input: &Input,
                commands: &mut [Option<Command>],
                barricade_assigns: &mut [VecDeque<PlacementAssign>],
                next_placeable_map: &mut Vec<Vec<bool>>,
                next_barricade_map: &mut Vec<Vec<bool>>,
            ) {
                if barricade_assigns.iter().all(|v| v.len() == 0) {
                    return;
                }

                to_commands(
                    input,
                    commands,
                    barricade_assigns,
                    next_placeable_map,
                    next_barricade_map,
                );
            }

            fn to_commands(
                input: &Input,
                commands: &mut [Option<Command>],
                barricade_assigns: &mut [VecDeque<PlacementAssign>],
                next_placeable_map: &mut Vec<Vec<bool>>,
                next_barricade_map: &mut Vec<Vec<bool>>,
            ) {
                for ((assign, &person), command) in barricade_assigns
                    .iter_mut()
                    .zip(input.people.iter())
                    .zip(commands.iter_mut())
                {
                    'person: while let Some((next_pivot, direction)) = assign.front_mut() {
                        if *direction == 0 {
                            assign.pop_front();
                            continue;
                        }

                        if person != *next_pivot {
                            let route =
                                common::calculate_route(&next_barricade_map, person, *next_pivot);

                            if let Some(route) = route {
                                let first_move = route[0];
                                let next = person + first_move;
                                next_placeable_map[next.row][next.col] = false;
                                *command = Some(Command::Move(first_move));
                                break;
                            } else {
                                eprintln!(
                                    "[WARNING!] can't move to ({}, {})",
                                    next_pivot.row, next_pivot.col
                                );
                                assign.pop_front();
                            }
                        } else {
                            let mut completed = true;

                            for (dir, &adj) in ADJACENTS.iter().enumerate() {
                                let next = person + adj;
                                if (*direction & (1 << dir)) == 0 {
                                    continue;
                                } else if next_barricade_map[next.row][next.col] {
                                    // 他の人に置かれている場合
                                    *direction &= !(1 << dir);
                                }

                                completed = false;

                                if next_placeable_map[next.row][next.col] {
                                    *command = Some(Command::Put(adj));
                                    next_barricade_map[next.row][next.col] = true;
                                    next_placeable_map[next.row][next.col] = false;
                                    *direction &= !(1 << dir);
                                    break 'person;
                                }
                            }

                            if !completed && command.is_none() {
                                *command = Some(Command::None);
                                break;
                            }
                        }
                    }
                }
            }
        }

        /// 定位置の列に移動する
        mod second_phase {
            use std::collections::VecDeque;

            use crate::{
                acl::maxflow::MfGraph,
                ahc008::configs::Config,
                ahc008::{algorithms::common, Command, Input, PlacementAssign, MAP_SIZE},
                grid::Coordinate,
            };

            pub(super) fn get_commands(
                input: &Input,
                config: &Config,
                commands: &mut [Option<Command>],
                barricade_assigns: &[VecDeque<PlacementAssign>],
                next_barricade_map: &mut Vec<Vec<bool>>,
            ) {
                let matchings = get_matchings(input, config, barricade_assigns, next_barricade_map);

                if let Some(matchings) = matchings {
                    to_commands(input, &matchings, commands, next_barricade_map);
                }
            }

            fn get_matchings(
                input: &Input,
                config: &Config,
                barricade_assigns: &[VecDeque<PlacementAssign>],
                next_barricade_map: &Vec<Vec<bool>>,
            ) -> Option<Vec<usize>> {
                let mut candidates = config.aisle_cols.clone();
                candidates.sort_by_key(|&c| (c as i32 - MAP_SIZE as i32 / 2).abs());

                let mut columns = vec![0; input.people_count];

                for (i, column) in columns.iter_mut().enumerate() {
                    *column = candidates[i % candidates.len()];
                }

                let mut graph = MfGraph::<i32>::new(input.people_count * 2 + 2);
                let cols_offset = input.people_count;
                let source = cols_offset + input.people_count;
                let sink = source + 1;

                // source -> 人i
                for i in 0..input.people_count {
                    graph.add_edge(source, i, 1);
                }

                // 列j -> sink
                for j in 0..input.people_count {
                    graph.add_edge(j + cols_offset, sink, 1);
                }

                let edge_offset = input.people_count * 2;
                let matching_edges = get_matching_edges(
                    input,
                    config,
                    barricade_assigns,
                    next_barricade_map,
                    &columns,
                );
                let mut edge_index = 0;
                let mut needed_time = 0;
                let mut flow = 0;

                // 蟻本.206
                loop {
                    while edge_index < matching_edges.len()
                        && matching_edges[edge_index].0 <= needed_time
                    {
                        let (_, i, j) = matching_edges[edge_index];

                        graph.add_edge(i, j + cols_offset, 1);
                        edge_index += 1;
                    }

                    flow += graph.flow(source, sink);

                    if flow == input.people_count as i32 {
                        if needed_time == 0 {
                            // 全員到着済み
                            return None;
                        }

                        // マッチングを求める
                        let mut matchings = vec![std::usize::MAX; input.people_count];

                        for (index, edge) in graph.edges().iter().skip(edge_offset).enumerate() {
                            let (_, i, j) = matching_edges[index];
                            if edge.flow > 0 {
                                matchings[i] = columns[j];
                            }
                        }

                        return Some(matchings);
                    }

                    needed_time += 1;
                }
            }

            fn get_matching_edges(
                input: &Input,
                config: &Config,
                barricade_assigns: &[VecDeque<PlacementAssign>],
                next_barricade_map: &Vec<Vec<bool>>,
                columns: &[usize],
            ) -> Vec<(i32, usize, usize)> {
                let mut edges = vec![];

                // バリケードは完成したものを対象とする
                let mut next_barricade_map = next_barricade_map.clone();

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        if config.map[row][col] == b'X' {
                            next_barricade_map[row][col] = true;
                        }
                    }
                }

                for (i, (&person, queue)) in input
                    .people
                    .iter()
                    .zip(barricade_assigns.iter())
                    .enumerate()
                {
                    let (start, init_cost) = if let Some(&(last, _)) = queue.back() {
                        // 置き終わるまでのターン数
                        let mut cost = 0;
                        let mut current = person;

                        for &(c, flag) in queue.iter() {
                            cost += current.distance_to(c);
                            cost += flag.count_ones() as i32;
                            current = c;
                        }

                        // このターン分
                        cost += 1;

                        (last, cost)
                    } else {
                        (person, 0)
                    };

                    let distances = common::calculate_distances(&next_barricade_map, start);

                    for (j, &col) in columns.iter().enumerate() {
                        let mut nearest_dist = std::i32::MAX / 2;

                        for row in 0..MAP_SIZE {
                            chmin!(nearest_dist, distances[row][col]);
                        }

                        edges.push((nearest_dist + init_cost, i, j));
                    }
                }

                edges.sort_unstable();
                edges
            }

            fn to_commands(
                input: &Input,
                matchings: &[usize],
                commands: &mut [Option<Command>],
                next_barricade_map: &Vec<Vec<bool>>,
            ) {
                for (&person, (&col, command)) in input
                    .people
                    .iter()
                    .zip(matchings.iter().zip(commands.iter_mut()))
                {
                    if command.is_some() {
                        continue;
                    }

                    let mut best_dist = std::i32::MAX;
                    let mut best_row = 0;
                    let distances = common::calculate_distances(next_barricade_map, person);

                    for row in 0..MAP_SIZE {
                        if chmin!(best_dist, distances[row][col]) {
                            best_row = row;
                        }
                    }

                    if best_dist > 0 {
                        let goal = Coordinate::new(best_row, col);
                        let route =
                            common::calculate_route(&next_barricade_map, person, goal).unwrap();
                        let first_move = route[0];
                        *command = Some(Command::Move(first_move));
                    }
                }
            }
        }

        /// 捕獲を実施する
        mod third_phase {
            use std::collections::VecDeque;

            use rand::{prelude::SliceRandom, Rng};
            use rand_pcg::Pcg64Mcg;

            use crate::{
                ahc008::configs::Config,
                ahc008::{algorithms::common, Command, Input, Pet, MAP_SIZE},
                grid::{self, Coordinate, ADJACENTS},
            };

            type Matching = (
                i64,
                Option<Coordinate>,
                Option<Coordinate>,
                usize,
                usize,
                usize,
            );

            #[derive(Debug, Clone)]
            struct Room {
                left: Option<Coordinate>,
                right: Option<Coordinate>,
                pet_counts: Vec<usize>,
                size: usize,
            }

            impl Room {
                fn new(
                    left: Option<Coordinate>,
                    right: Option<Coordinate>,
                    pet_counts: Vec<usize>,
                    size: usize,
                ) -> Self {
                    Self {
                        left,
                        right,
                        pet_counts,
                        size,
                    }
                }
            }

            pub(super) fn get_commands(
                input: &Input,
                config: &Config,
                next_barricade_map: &Vec<Vec<bool>>,
                next_placeable_map: &Vec<Vec<bool>>,
                commands: &mut [Option<Command>],
                rng: &mut Pcg64Mcg,
            ) {
                if commands.iter().all(|c| c.is_some()) {
                    return;
                }

                let candidates = get_placement_candidates(
                    input,
                    config,
                    next_barricade_map,
                    next_placeable_map,
                    commands,
                    rng,
                );
                let matchings = max_matching(input, &candidates);
                to_commands(input, &matchings, commands);
            }

            fn get_placement_candidates(
                input: &Input,
                config: &Config,
                next_barricade_map: &Vec<Vec<bool>>,
                next_placeable_map: &Vec<Vec<bool>>,
                commands: &[Option<Command>],
                rng: &mut Pcg64Mcg,
            ) -> Vec<Matching> {
                const COEF: f64 = 1e8;
                let mut candidates = vec![];

                let predictions = predict_pet_moves(&input, config, next_barricade_map, rng);

                for (id, prediction) in predictions.iter().enumerate() {
                    match (prediction.left, prediction.right) {
                        (Some(door1), Some(door2)) => {
                            let penalty = side_penalty(door1.row);
                            let waiting_turn = if !next_placeable_map[door1.row][door1.col]
                                || !next_placeable_map[door2.row][door2.col]
                            {
                                1
                            } else {
                                0
                            };

                            for (i, &p1) in input.people.iter().enumerate() {
                                if p1.col != door1.col - 1 || commands[i].is_some() {
                                    continue;
                                }

                                for (j, &p2) in input.people.iter().enumerate() {
                                    if p2.col != door2.col + 1 || commands[j].is_some() {
                                        continue;
                                    }
                                    let dist = dist_abs(p1.row, door1.row)
                                        .max(dist_abs(p2.row, door2.row))
                                        .max(waiting_turn);
                                    let score = calculate_score(dist, &prediction, penalty);

                                    if score > 0 {
                                        candidates.push((
                                            score,
                                            Some(door1),
                                            Some(door2),
                                            i,
                                            j,
                                            id,
                                        ));
                                    }
                                }
                            }
                        }
                        (Some(door1), None) => {
                            let penalty = side_penalty(door1.row);
                            let waiting_turn = if !next_placeable_map[door1.row][door1.col] {
                                1
                            } else {
                                0
                            };

                            for (i, &p1) in input.people.iter().enumerate() {
                                if p1.col != door1.col - 1 || commands[i].is_some() {
                                    continue;
                                }

                                let dist = dist_abs(p1.row, door1.row).max(waiting_turn);
                                let score = calculate_score(dist, &prediction, penalty);

                                if score > 0 {
                                    candidates.push((score, Some(door1), None, i, i, id));
                                }
                            }
                        }
                        (None, Some(door2)) => {
                            let penalty = side_penalty(door2.row);
                            let waiting_turn = if !next_placeable_map[door2.row][door2.col] {
                                1
                            } else {
                                0
                            };

                            for (j, &p2) in input.people.iter().enumerate() {
                                if p2.col != door2.col + 1 || commands[j].is_some() {
                                    continue;
                                }

                                let dist = dist_abs(p2.row, door2.row).max(waiting_turn);
                                let score = calculate_score(dist, &prediction, penalty);

                                if score > 0 {
                                    candidates.push((score, None, Some(door2), j, j, id));
                                }
                            }
                        }
                        (None, None) => (),
                    }
                }

                candidates.sort_unstable();
                candidates.reverse();

                return candidates;

                fn dist_abs(row1: usize, row2: usize) -> i32 {
                    (row1 as i32 - row2 as i32).abs()
                }

                fn side_penalty(row: usize) -> f64 {
                    1.0 - (row as f64 - 15.5).abs() / 30.0
                }

                fn calculate_score(dist: i32, prediction: &Room, penalty: f64) -> i64 {
                    let div = (dist + 1) as f64;
                    let pet_count = prediction.pet_counts[dist as usize] as f64;
                    let room_size = prediction.size as f64;
                    let score = (pet_count * COEF * penalty / (room_size * div)) as i64;
                    score
                }
            }

            fn predict_pet_moves(
                input: &Input,
                config: &Config,
                next_barricade_map: &Vec<Vec<bool>>,
                rng: &mut Pcg64Mcg,
            ) -> Vec<Room> {
                const TURN_LENGTH: usize = 30;
                const ITER_COUNT: usize = 100;

                // 対象のペット
                let (connection_map, group_count) = common::get_connection_map(input);
                let majority_group =
                    common::get_majority_people_group(input, &connection_map, group_count);
                let alive_pet_indices =
                    common::get_alive_pet_indices(input, &connection_map, majority_group);

                // 部屋の定義
                let (room_ids, room_sizes, doors, room_count) =
                    get_room_numbers(input.clone(), config, next_barricade_map.clone());

                // 人々からの距離
                let people_distances = input
                    .people
                    .iter()
                    .map(|&p| get_distance_map(p, next_barricade_map))
                    .collect::<Vec<_>>();

                // モンテカルロ法でターンごとの期待値を計算
                // counts[turn][room]
                let monte_carlo_counts = monte_carlo_all(
                    input,
                    &alive_pet_indices,
                    &room_ids,
                    room_count,
                    &people_distances,
                    next_barricade_map,
                    TURN_LENGTH,
                    ITER_COUNT,
                    rng,
                );

                // 関節点
                let articulations = get_articulation_map(config, next_barricade_map);

                let mut results = vec![];

                'room_loop: for i in 0..room_count {
                    let (left_door, right_door) = doors[i];

                    // 関節点だったり既に壁があったりしたらNG
                    if let Some(door) = left_door {
                        if articulations[door.row][door.col]
                            || next_barricade_map[door.row][door.col]
                        {
                            continue;
                        }
                    }

                    if let Some(door) = right_door {
                        if articulations[door.row][door.col]
                            || next_barricade_map[door.row][door.col]
                        {
                            continue;
                        }
                    }

                    // 人がいたらNG
                    for person in input.people.iter() {
                        if room_ids[person.row][person.col] == i {
                            continue 'room_loop;
                        }
                    }

                    let mut counts = Vec::with_capacity(monte_carlo_counts.len());

                    for count in monte_carlo_counts.iter() {
                        counts.push(count[i]);
                    }

                    results.push(Room::new(left_door, right_door, counts, room_sizes[i]));
                }

                results
            }

            fn get_room_numbers(
                mut input: Input,
                config: &Config,
                mut next_barricade_map: Vec<Vec<bool>>,
            ) -> (
                Vec<Vec<usize>>,
                Vec<usize>,
                Vec<(Option<Coordinate>, Option<Coordinate>)>,
                usize,
            ) {
                // 閉じられた部屋は埋める
                let (connection_map, group_count) = common::get_connection_map(&input);
                let majority_group =
                    common::get_majority_people_group(&input, &connection_map, group_count);
                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        if connection_map[row][col] != majority_group {
                            next_barricade_map[row][col] = true;
                        }
                    }
                }

                let mut virtual_barricades = vec![];

                // ドアを埋める
                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        let c = config.map[row][col];
                        let b1 = next_barricade_map[row][col];
                        let b2 = c == b'(' || c == b')' || c == b'<' || c == b'>';

                        if b1 || b2 {
                            virtual_barricades.push(Coordinate::new(row, col));
                        }
                    }
                }

                input.barricades = virtual_barricades;
                let (mut connection_map, group_count) = common::get_connection_map(&input);

                // 通路のidをメモる
                let mut aisle_ids = vec![];

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        if config.map[row][col] == b'|' {
                            aisle_ids.push(connection_map[row][col]);
                        }
                    }
                }

                aisle_ids.sort_unstable();
                aisle_ids.dedup();

                // 通路を消して番号を振り直し
                let mut id = 0;
                let mut ids = vec![std::usize::MAX; group_count];

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        let c = connection_map[row][col];
                        if c < ids.len()
                            && ids[c] == std::usize::MAX
                            && aisle_ids.binary_search(&c).is_err()
                        {
                            ids[c] = id;
                            id += 1;
                        }
                    }
                }

                let group_count = group_count - aisle_ids.len();

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        let c = connection_map[row][col];
                        if c < ids.len() {
                            connection_map[row][col] = ids[c];
                        }
                    }
                }

                // ドアも塗る
                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        let c = config.map[row][col];

                        if c == b'(' || c == b'<' {
                            connection_map[row][col] = connection_map[row][col + 1];
                        } else if c == b')' || c == b'>' {
                            connection_map[row][col] = connection_map[row][col - 1];
                        }
                    }
                }

                // 各部屋の広さ
                let mut sizes = vec![0; group_count];

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        let g = connection_map[row][col];

                        if g < sizes.len() {
                            sizes[g] += 1;
                        }
                    }
                }

                // ドアの定義
                let mut left_doors = vec![None; group_count];
                let mut right_doors = vec![None; group_count];
                let mut seen = mat![false; MAP_SIZE; MAP_SIZE];
                let mut stack = vec![];

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        let group_id = connection_map[row][col];
                        let c = config.map[row][col];

                        if c == b'(' || c == b')' || c == b'<' || c == b'>' {
                            continue;
                        }

                        if !seen[row][col] && group_id < group_count {
                            seen[row][col] = true;
                            stack.push(Coordinate::new(row, col));

                            while let Some(c) = stack.pop() {
                                for &adj in ADJACENTS.iter() {
                                    let next = c + adj;
                                    let next_c = config.map[next.row][next.col];

                                    if seen[next.row][next.col]
                                        || next_barricade_map[next.row][next.col]
                                    {
                                        continue;
                                    }

                                    if next_c == b'(' || next_c == b'<' {
                                        left_doors[group_id] = Some(next);
                                    } else if next_c == b')' || next_c == b'>' {
                                        right_doors[group_id] = Some(next);
                                    } else {
                                        seen[next.row][next.col] = true;
                                        stack.push(next);
                                    }
                                }
                            }
                        }
                    }
                }

                let doors = left_doors
                    .iter()
                    .zip(right_doors.iter())
                    .map(|(&left, &right)| (left, right))
                    .collect();

                (connection_map, sizes, doors, group_count)
            }

            fn get_articulation_map(
                config: &Config,
                next_barricade_map: &Vec<Vec<bool>>,
            ) -> Vec<Vec<bool>> {
                // 普通に関節点を求めてから、袋小路に至る道を関節点判定から外す
                const NONE: Coordinate = Coordinate::new(!0, !0);

                // 関節点
                let mut articulations = vec![];
                let mut used = mat![false; MAP_SIZE; MAP_SIZE];
                let mut ord = mat![0; MAP_SIZE; MAP_SIZE];
                let mut low = mat![0; MAP_SIZE; MAP_SIZE];
                let mut id = 0;

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        if !next_barricade_map[row][col] && !used[row][col] {
                            let c = Coordinate::new(row, col);
                            id = dfs(
                                c,
                                NONE,
                                id,
                                &mut articulations,
                                &mut used,
                                &mut ord,
                                &mut low,
                                next_barricade_map,
                            );
                        }
                    }
                }

                // マップに落とす
                let mut articulation_map = mat![false; MAP_SIZE; MAP_SIZE];

                for articulation in articulations {
                    articulation_map[articulation.row][articulation.col] = true;
                }

                // 袋小路を潰す
                // 横に舐めるだけなので1行目は漏れてしまうが、興味があるのは入口だけなので問題ない
                let mut stack = vec![];
                let map = &config.map;

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        if is_left(map[row][col]) {
                            // 右方向に舐める
                            let mut col = col;

                            while !is_barricade(map[row][col]) && !is_right(map[row][col]) {
                                stack.push((row, col));
                                col += 1;
                            }

                            // バックトラック
                            if is_barricade(map[row][col]) {
                                while let Some((row, col)) = stack.pop() {
                                    articulation_map[row][col] = false;
                                }
                            } else {
                                stack.clear();
                            }
                        } else if is_right(map[row][col]) {
                            // 左方向に舐める
                            let mut col = col;

                            while !is_barricade(map[row][col]) && !is_left(map[row][col]) {
                                stack.push((row, col));
                                col -= 1;
                            }

                            // バックトラック
                            if is_barricade(map[row][col]) {
                                while let Some((row, col)) = stack.pop() {
                                    articulation_map[row][col] = false;
                                }
                            } else {
                                stack.clear();
                            }
                        }
                    }
                }

                return articulation_map;

                fn dfs(
                    current: Coordinate,
                    parent: Coordinate,
                    mut id: usize,
                    articulations: &mut Vec<Coordinate>,
                    used: &mut Vec<Vec<bool>>,
                    ord: &mut Vec<Vec<usize>>,
                    low: &mut Vec<Vec<usize>>,
                    barricade_map: &Vec<Vec<bool>>,
                ) -> usize {
                    // lowlink
                    used[current.row][current.col] = true;
                    ord[current.row][current.col] = id;
                    low[current.row][current.col] = ord[current.row][current.col];
                    id += 1;

                    let mut is_articulation = false;
                    let mut children_count = 0;

                    for &adj in ADJACENTS.iter() {
                        let next = current + adj;

                        if barricade_map[next.row][next.col] {
                            continue;
                        }

                        if !used[next.row][next.col] {
                            children_count += 1;
                            id = dfs(
                                next,
                                current,
                                id,
                                articulations,
                                used,
                                ord,
                                low,
                                barricade_map,
                            );
                            chmin!(low[current.row][current.col], low[next.row][next.col]);

                            if parent != NONE
                                && ord[current.row][current.col] <= low[next.row][next.col]
                            {
                                is_articulation = true;
                            }
                        } else if next != parent {
                            chmin!(low[current.row][current.col], ord[next.row][next.col]);
                        }
                    }

                    if parent == NONE && children_count >= 2 {
                        is_articulation = true;
                    }

                    if is_articulation {
                        articulations.push(current);
                    }

                    return id;
                }

                fn is_left(c: u8) -> bool {
                    c == b'(' || c == b'<'
                }

                fn is_right(c: u8) -> bool {
                    c == b')' || c == b'>'
                }

                fn is_barricade(c: u8) -> bool {
                    c == b'#' || c == b'X'
                }
            }

            fn get_distance_map(
                start: Coordinate,
                barricade_map: &Vec<Vec<bool>>,
            ) -> Vec<Vec<i32>> {
                let mut distances = mat![std::i32::MAX; MAP_SIZE; MAP_SIZE];
                let mut queue = VecDeque::new();
                distances[start.row][start.col] = 0;
                queue.push_back(start);

                while let Some(coordinate) = queue.pop_front() {
                    let dist = distances[coordinate.row][coordinate.col];

                    for &adj in ADJACENTS.iter() {
                        let next = coordinate + adj;
                        let next_dist = dist + 1;

                        if !barricade_map[next.row][next.col]
                            && chmin!(distances[next.row][next.col], next_dist)
                        {
                            queue.push_back(next);
                        }
                    }
                }

                distances
            }

            fn monte_carlo_all(
                input: &Input,
                pet_indices: &[usize],
                room_ids: &Vec<Vec<usize>>,
                room_count: usize,
                people_distances: &Vec<Vec<Vec<i32>>>,
                barricade_map: &Vec<Vec<bool>>,
                turn_length: usize,
                iter_count: usize,
                rng: &mut Pcg64Mcg,
            ) -> Vec<Vec<usize>> {
                let mut counts = mat![0; turn_length; room_count];

                for pet in pet_indices.iter().map(|&i| input.pets[i]) {
                    let c = pet.coordinate;
                    let init_room_id = room_ids[c.row][c.col];

                    if init_room_id < room_count {
                        for _ in 0..iter_count {
                            monte_carlo(
                                input,
                                pet,
                                room_ids,
                                init_room_id,
                                people_distances,
                                barricade_map,
                                &mut counts,
                                rng,
                            );
                        }
                    }
                }

                counts
            }

            fn monte_carlo(
                input: &Input,
                mut pet: Pet,
                room_ids: &Vec<Vec<usize>>,
                init_room_id: usize,
                people_distances: &Vec<Vec<Vec<i32>>>,
                barricade_map: &Vec<Vec<bool>>,
                counts: &mut [Vec<usize>],
                rng: &mut Pcg64Mcg,
            ) {
                let mut candidates = vec![];
                let mut target_person = !0;

                for counts in counts.iter_mut() {
                    let id = room_ids[pet.coordinate.row][pet.coordinate.col];
                    if id == init_room_id {
                        counts[id] += 1;
                    }

                    move_pet(
                        input,
                        &mut pet,
                        &mut target_person,
                        &mut candidates,
                        barricade_map,
                        people_distances,
                        rng,
                    );
                }

                fn move_pet(
                    input: &Input,
                    pet: &mut Pet,
                    target_person: &mut usize,
                    candidates: &mut Vec<Coordinate>,
                    barricade_map: &Vec<Vec<bool>>,
                    people_distances: &Vec<Vec<Vec<i32>>>,
                    rng: &mut rand_pcg::Mcg128Xsl64,
                ) {
                    match pet.pet_type {
                        crate::ahc008::PetType::Cow => {
                            pet.coordinate =
                                standard_move(pet.coordinate, barricade_map, candidates, rng);
                        }
                        crate::ahc008::PetType::Pig => {
                            for _ in 0..2 {
                                pet.coordinate =
                                    standard_move(pet.coordinate, barricade_map, candidates, rng);
                            }
                        }
                        crate::ahc008::PetType::Rabbit => {
                            for _ in 0..3 {
                                pet.coordinate =
                                    standard_move(pet.coordinate, barricade_map, candidates, rng);
                            }
                        }
                        crate::ahc008::PetType::Dog => {
                            let c = pet.coordinate;

                            while *target_person == !0 {
                                let target = rng.gen_range(0, input.people_count);
                                let dist = people_distances[target][c.row][c.col];
                                if 0 < dist && dist < std::i32::MAX {
                                    *target_person = target;
                                }
                            }

                            // 近付く
                            candidates.clear();
                            let distances = &people_distances[*target_person];

                            for &adj in ADJACENTS.iter() {
                                let next = c + adj;
                                if distances[next.row][next.col] < distances[c.row][c.col] {
                                    candidates.push(next);
                                }
                            }

                            pet.coordinate = *candidates.choose(rng).unwrap();
                            let c = pet.coordinate;

                            if distances[c.row][c.col] == 0 {
                                *target_person = !0;
                            }

                            pet.coordinate =
                                standard_move(pet.coordinate, barricade_map, candidates, rng);

                            let c = pet.coordinate;

                            if distances[c.row][c.col] == 0 {
                                *target_person = !0;
                            }
                        }
                        crate::ahc008::PetType::Cat => {
                            // 真面目に目的地を計算すると計算量がヤバいのでサボる
                            for _ in 0..2 {
                                pet.coordinate =
                                    standard_move(pet.coordinate, barricade_map, candidates, rng);
                            }
                        }
                    }
                }

                fn standard_move(
                    pet: Coordinate,
                    barricade_map: &Vec<Vec<bool>>,
                    candidates: &mut Vec<Coordinate>,
                    rng: &mut Pcg64Mcg,
                ) -> Coordinate {
                    candidates.clear();

                    for &adj in ADJACENTS.iter() {
                        let next = pet + adj;
                        if !barricade_map[next.row][next.col] {
                            candidates.push(next);
                        }
                    }

                    *candidates.choose(rng).unwrap()
                }
            }

            fn max_matching(input: &Input, candidates: &[Matching]) -> Vec<Matching> {
                let mut best_score = 0;
                let mut best_result = vec![];
                let mut current_result = vec![];
                let mut prefix_sum = vec![0];

                for (i, &(score, _, _, _, _, _)) in candidates.iter().enumerate() {
                    prefix_sum.push(prefix_sum[i] + score);
                }

                // 番兵を置いておく
                let last = *prefix_sum.last().unwrap();
                for _ in 0..20 {
                    prefix_sum.push(last);
                }

                let max_selection = input.people_count;
                dfs(
                    0,
                    &mut current_result,
                    &mut best_score,
                    &mut best_result,
                    candidates,
                    &prefix_sum,
                    0,
                    0,
                    max_selection,
                    input,
                    0,
                );

                return best_result;

                fn dfs(
                    current_score: i64,
                    current_result: &mut Vec<Matching>,
                    best_score: &mut i64,
                    best_result: &mut Vec<Matching>,
                    candidates: &[Matching],
                    prefix_sum: &[i64],
                    people_flag: u32,
                    room_flag: u128,
                    max_selection: usize,
                    input: &Input,
                    depth: usize,
                ) {
                    if depth == candidates.len() {
                        if chmax!(*best_score, current_score) {
                            *best_result = current_result.clone();
                        }

                        return;
                    }

                    // 嘘枝刈り（なぜか点数が上がる）
                    let remaining_pair =
                        (input.people_count - people_flag.count_ones() as usize) / 2;

                    if current_score + prefix_sum[depth + remaining_pair] - prefix_sum[depth]
                        <= *best_score
                    {
                        return;
                    }

                    let (score, _, _, i, j, room) = candidates[depth];

                    let p_flag = (1 << i) | (1 << j);
                    let r_flag = 1 << room;

                    // 使う
                    if (people_flag & p_flag) == 0 && (room_flag & r_flag) == 0 {
                        current_result.push(candidates[depth]);
                        dfs(
                            current_score + score,
                            current_result,
                            best_score,
                            best_result,
                            candidates,
                            prefix_sum,
                            people_flag | p_flag,
                            room_flag | r_flag,
                            max_selection,
                            input,
                            depth + 1,
                        );
                        current_result.pop();
                    }

                    // 使わない
                    dfs(
                        current_score,
                        current_result,
                        best_score,
                        best_result,
                        candidates,
                        prefix_sum,
                        people_flag,
                        room_flag,
                        max_selection,
                        input,
                        depth + 1,
                    );
                }
            }

            fn to_commands(
                input: &Input,
                matchings: &[Matching],
                commands: &mut [Option<Command>],
            ) {
                let placeable_map = input.get_placeable_map();

                for &(_, door1, door2, index1, index2, _) in matchings.iter() {
                    let person1 = input.people[index1];
                    let person2 = input.people[index2];

                    match (door1, door2) {
                        (Some(door1), Some(door2)) => {
                            if person1.row == door1.row && person2.row == door2.row {
                                if placeable_map[door1.row][door1.col]
                                    && placeable_map[door2.row][door2.col]
                                {
                                    commands[index1] = Some(Command::Put(grid::char_to_adj('R')));
                                    commands[index2] = Some(Command::Put(grid::char_to_adj('L')));
                                } else {
                                    // 置けない場合はその場で待機
                                    commands[index1] = Some(Command::None);
                                    commands[index2] = Some(Command::None);
                                }
                            } else {
                                commands[index1] = Some(to_move(person1.row, door1.row));
                                commands[index2] = Some(to_move(person2.row, door2.row));
                            }
                        }
                        (Some(door1), None) => {
                            if person1.row == door1.row {
                                if placeable_map[door1.row][door1.col] {
                                    commands[index1] = Some(Command::Put(grid::char_to_adj('R')));
                                } else {
                                    // 置けない場合はその場で待機
                                    commands[index1] = Some(Command::None);
                                }
                            } else {
                                commands[index1] = Some(to_move(person1.row, door1.row));
                            }
                        }
                        (None, Some(door2)) => {
                            if person2.row == door2.row {
                                if placeable_map[door2.row][door2.col] {
                                    commands[index2] = Some(Command::Put(grid::char_to_adj('L')));
                                } else {
                                    // 置けない場合はその場で待機
                                    commands[index2] = Some(Command::None);
                                }
                            } else {
                                commands[index2] = Some(to_move(person2.row, door2.row));
                            }
                        }
                        (None, None) => unreachable!(),
                    }
                }

                fn to_move(current_row: usize, target_row: usize) -> Command {
                    if current_row > target_row {
                        Command::Move(grid::char_to_adj('U'))
                    } else if current_row < target_row {
                        Command::Move(grid::char_to_adj('D'))
                    } else {
                        Command::None
                    }
                }
            }
        }

        /// 暇なスタッフはペットと横軸を合わせる
        mod fourth_phase {
            use crate::{
                acl::mincostflow::MinCostFlowGraph,
                ahc008::configs::Config,
                ahc008::{Command, Input},
                grid,
            };

            use super::common;

            const MAX_DIST: i32 = 30;

            pub(super) fn get_commands(
                input: &Input,
                config: &Config,
                commands: &mut [Option<Command>],
            ) {
                let input = create_next_input(input.clone(), commands.to_vec());
                let distances = get_distances(&input, commands);
                let matchings = get_matchings(&input, config, commands, &distances);
                to_commands(&input, &matchings, commands);
            }

            fn create_next_input(mut input: Input, commands: Vec<Option<Command>>) -> Input {
                let commands = commands
                    .iter()
                    .map(|&c| {
                        // Putだけを有効とする
                        let c = if let Some(Command::Put(_)) = c {
                            c.unwrap()
                        } else {
                            Command::None
                        };
                        c
                    })
                    .collect::<Vec<_>>();
                input.update_people(&commands);
                input
            }

            fn get_distances(input: &Input, commands: &[Option<Command>]) -> Vec<Vec<i32>> {
                let mut distances = mat![std::i32::MAX; input.people_count; input.pet_count];
                let barricade_map = input.get_barricade_map();

                for (i, &person) in input.people.iter().enumerate() {
                    if commands[i].is_some() {
                        continue;
                    }

                    let dist = common::calculate_distances(&barricade_map, person);

                    for (j, pet) in input.pets.iter().enumerate() {
                        let c = pet.coordinate;
                        distances[i][j] = dist[c.row][c.col];
                    }
                }

                distances
            }

            fn get_matchings(
                input: &Input,
                config: &Config,
                commands: &[Option<Command>],
                distances: &Vec<Vec<i32>>,
            ) -> Vec<Option<usize>> {
                let mut matchings = vec![None; input.people_count];

                for &col in config.aisle_cols.iter() {
                    let mut graph = MinCostFlowGraph::new(input.people_count + input.pet_count + 2);
                    let pet_offset = input.people_count;
                    let source = pet_offset + input.pet_count;
                    let sink = source + 1;

                    // 人 -> ペット
                    let mut pairs = vec![];
                    for i in 0..input.people_count {
                        if input.people[i].col != col || commands[i].is_some() {
                            continue;
                        }

                        for j in 0..input.pet_count {
                            let d = distances[i][j];

                            if d <= MAX_DIST {
                                graph.add_edge(i, j + pet_offset, 1, d);
                                pairs.push((i, j));
                            }
                        }
                    }

                    // source -> 人
                    for i in 0..input.people_count {
                        graph.add_edge(source, i, 1, 0);
                    }

                    // 人 -> sink
                    for j in 0..input.pet_count {
                        graph.add_edge(j + pet_offset, sink, 1, 0);
                    }

                    graph.flow(source, sink, std::i32::MAX);

                    for (index, &(i, j)) in pairs.iter().enumerate() {
                        let edge = graph.get_edge(index);
                        if edge.flow > 0 {
                            matchings[i] = Some(input.pets[j].coordinate.row);
                        }
                    }
                }

                matchings
            }

            fn to_commands(
                input: &Input,
                matchings: &Vec<Option<usize>>,
                commands: &mut [Option<Command>],
            ) {
                let barricade_map = input.get_barricade_map();

                for (&person, (&matching, command)) in input
                    .people
                    .iter()
                    .zip(matchings.iter().zip(commands.iter_mut()))
                {
                    if let Some(row) = matching {
                        if person.row > row {
                            let adj = grid::char_to_adj('U');
                            let next = person + adj;

                            if !barricade_map[next.row][next.col] {
                                *command = Some(Command::Move(adj));
                            }
                        }

                        if person.row < row {
                            let adj = grid::char_to_adj('D');
                            let next = person + adj;

                            if !barricade_map[next.row][next.col] {
                                *command = Some(Command::Move(adj));
                            }
                        }
                    } else if command.is_none() {
                        // 打ったら戻る、真ん中へ
                        if person.row >= 17 {
                            let adj = grid::char_to_adj('U');
                            let next = person + adj;

                            if !barricade_map[next.row][next.col] {
                                *command = Some(Command::Move(adj));
                            }
                        } else if person.row <= 14 {
                            let adj = grid::char_to_adj('D');
                            let next = person + adj;

                            if !barricade_map[next.row][next.col] {
                                *command = Some(Command::Move(adj));
                            }
                        }
                    }
                }
            }
        }

        /// 自分から動いて捕まえに行く
        mod fifth_phase {
            use crate::{
                acl::{maxflow::MfGraph, mincostflow::MinCostFlowGraph},
                ahc008::{configs::Config, Command, Input, MAP_SIZE, MAX_TURN},
                grid::{Coordinate, CoordinateDiff, ADJACENTS},
            };

            use super::common;

            pub(super) fn get_commands(
                input: &Input,
                config: &Config,
                turn: usize,
                commands: &mut [Option<Command>],
                next_placeable_map: &mut Vec<Vec<bool>>,
                next_barricade_map: &mut Vec<Vec<bool>>,
            ) {
                let (connection_map, group_count) = common::get_connection_map(input);
                let majority_group =
                    common::get_majority_people_group(input, &connection_map, group_count);
                let target_pets =
                    common::get_alive_pet_indices(input, &connection_map, majority_group);
                if target_pets.len() == 0 {
                    return;
                }

                let (needed_barricades, evasion_map) =
                    max_cut(input, config, turn, &target_pets, next_barricade_map);

                let available = commands.iter().map(|c| c.is_none()).collect::<Vec<_>>();
                let matchings =
                    get_matchings(input, &available, &needed_barricades, &next_barricade_map);

                let is_reasonable_plan = check_reasonability(input.clone(), &needed_barricades);

                let mut routes = vec![None; input.people_count];

                // 逃げ道を作る
                for (i, (&person, &matching)) in
                    input.people.iter().zip(matchings.iter()).enumerate()
                {
                    if let Some(matching) = matching {
                        routes[i] = common::calculate_route(&next_barricade_map, person, matching);

                        // 最終ターンは無視
                        if evasion_map[person.row][person.col] && turn < MAX_TURN - 1 {
                            let mut current = person;

                            for route in routes[i].as_ref().unwrap().iter() {
                                current += *route;
                                next_placeable_map[current.row][current.col] = false;
                            }
                        }
                    }
                }

                for (i, (&person, command)) in
                    input.people.iter().zip(commands.iter_mut()).enumerate()
                {
                    if let Some(route) = routes[i].as_ref() {
                        if route.len() == 0 {
                            // 上に乗ってる場合は一旦どく
                            for &adj in ADJACENTS.iter() {
                                let next = person + adj;

                                if !evasion_map[next.row][next.col]
                                    && !next_barricade_map[next.row][next.col]
                                {
                                    *command = Some(Command::Move(adj));
                                    next_placeable_map[next.row][next.col] = false;
                                    next_barricade_map[next.row][next.col] = true;
                                    break;
                                }
                            }
                        } else if route.len() == 1 && !evasion_map[person.row][person.col] {
                            // 置く
                            let adj = route[0];
                            let next = person + adj;

                            if !next_barricade_map[next.row][next.col]
                                && next_placeable_map[next.row][next.col]
                                && is_reasonable_plan
                            {
                                *command = Some(Command::Put(adj));
                                next_placeable_map[next.row][next.col] = false;
                                next_barricade_map[next.row][next.col] = true;
                            }
                        } else {
                            // 近付く
                            let adj = route[0];
                            let next = person + adj;

                            if !next_barricade_map[next.row][next.col] {
                                *command = Some(Command::Move(adj));
                                next_placeable_map[next.row][next.col] = false;
                            }
                        }
                    }
                }

                //eprintln!("=========");
                //for &barricade in needed_barricades.iter() {
                //    eprintln!("({}, {})", barricade.row, barricade.col);
                //}
            }

            fn check_reasonability(mut input: Input, needed_barricades: &[Coordinate]) -> bool {
                const THRESHOLD: f64 = 0.95;

                let base_score = common::calculate_score(&input) as f64;

                for &barricade in needed_barricades.iter() {
                    input.barricades.push(barricade);
                }

                let new_score = common::calculate_score(&input) as f64;
                new_score >= base_score * THRESHOLD
            }

            fn max_cut(
                input: &Input,
                config: &Config,
                turn: usize,
                target_pets: &[usize],
                barricade_map: &Vec<Vec<bool>>,
            ) -> (Vec<Coordinate>, Vec<Vec<bool>>) {
                const NEIGHBOR_DIFFS_2: [CoordinateDiff; 13] = [
                    CoordinateDiff::new(!0 - 1, 0),
                    CoordinateDiff::new(!0, !0),
                    CoordinateDiff::new(!0, 0),
                    CoordinateDiff::new(!0, 1),
                    CoordinateDiff::new(0, !0 - 1),
                    CoordinateDiff::new(0, !0),
                    CoordinateDiff::new(0, 0),
                    CoordinateDiff::new(0, 1),
                    CoordinateDiff::new(0, 2),
                    CoordinateDiff::new(1, !0),
                    CoordinateDiff::new(1, 0),
                    CoordinateDiff::new(1, 1),
                    CoordinateDiff::new(2, 0),
                ];

                const INF: i32 = std::i32::MAX;
                const FRONT: usize = 0;
                const BACK: usize = 1;
                const BLOCK_MULTIPLYER: i32 = 5;

                let mut graph =
                    MfGraph::<i32>::new(MAP_SIZE * MAP_SIZE * 2 + target_pets.len() + 2);
                let pet_offset = MAP_SIZE * MAP_SIZE * 2;
                let source = pet_offset + target_pets.len();
                let sink = source + 1;

                let all_distances = common::calculate_all_distances(&input.people, &barricade_map);
                let dist_limit = if turn <= 295 {
                    MAX_TURN - turn - 2
                } else if turn < 299 {
                    MAX_TURN - turn - 1
                } else {
                    MAX_TURN - turn
                } as i32;

                // 頂点のカット
                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        let from = to_index(row, col, FRONT);
                        let to = to_index(row, col, BACK);
                        let reachable = all_distances.iter().any(|d| d[row][col] <= dist_limit);
                        let has_person = input.people.iter().any(|p| p.row == row && p.col == col);

                        let cap = if barricade_map[row][col] {
                            0
                        } else if reachable && !has_person {
                            BLOCK_MULTIPLYER
                        } else {
                            INF
                        };
                        graph.add_edge(from, to, cap);
                    }
                }

                // source -> aisle
                let mut total_flow = 0;

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        let coordinate = Coordinate::new(row, col);

                        if config.map[row][col] == b'|' {
                            let near_pet = target_pets
                                .iter()
                                .map(|&p| input.pets[p].coordinate)
                                .any(|p| coordinate.distance_to(p) <= 1);

                            if near_pet {
                                continue;
                            }

                            let to = to_index(row, col, FRONT);
                            graph.add_edge(source, to, 1);
                            total_flow += 1;
                        }
                    }
                }

                for &person in input.people.iter() {
                    let near_pet = target_pets
                        .iter()
                        .map(|&p| input.pets[p].coordinate)
                        .any(|p| person.distance_to(p) <= 1);

                    if near_pet {
                        continue;
                    }

                    let to = to_index(person.row, person.col, FRONT);
                    graph.add_edge(source, to, 1);
                    total_flow += 1;
                }

                // pet -> sink
                // TODO: 行き止まりを作らずペットに近い方でカットする方法を考える
                for (i, &pet_index) in target_pets.iter().enumerate() {
                    let pet = input.pets[pet_index].coordinate;
                    let distances = common::calculate_distances(&barricade_map, pet);

                    for &adj in NEIGHBOR_DIFFS_2.iter() {
                        let next = pet + adj;

                        if next.in_map(MAP_SIZE) && distances[next.row][next.col] <= 2 {
                            let from = to_index(next.row, next.col, BACK);
                            let to = pet_offset + i;
                            graph.add_edge(from, to, INF);
                        }
                    }

                    let from = pet_offset + i;
                    let pet_count = target_pets.len() as i32;
                    let cap = (total_flow + pet_count - 1) / pet_count;
                    graph.add_edge(from, sink, cap);
                }

                // back -> front
                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        for &adj in ADJACENTS.iter() {
                            let next = Coordinate::new(row, col) + adj;

                            if next.in_map(MAP_SIZE) {
                                let from = to_index(row, col, BACK);
                                let to = to_index(next.row, next.col, FRONT);
                                graph.add_edge(from, to, INF);
                            }
                        }
                    }
                }

                graph.flow(source, sink);
                let cut = graph.min_cut(source);

                let mut needed_barricades = vec![];
                let mut evasion_map = mat![false; MAP_SIZE; MAP_SIZE];

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        if barricade_map[row][col] {
                            evasion_map[row][col] = true;
                        } else {
                            let front = to_index(row, col, FRONT);
                            let back = to_index(row, col, BACK);

                            evasion_map[row][col] = !cut[front] || (cut[front] ^ cut[back]);
                            if cut[front] ^ cut[back] {
                                needed_barricades.push(Coordinate::new(row, col));
                            }
                        }
                    }
                }

                return (needed_barricades, evasion_map);

                fn to_index(row: usize, col: usize, side: usize) -> usize {
                    (row * MAP_SIZE + col) * 2 + side
                }
            }

            fn get_matchings(
                input: &Input,
                available: &[bool],
                needed_barricades: &[Coordinate],
                barricade_map: &Vec<Vec<bool>>,
            ) -> Vec<Option<Coordinate>> {
                let mut graph =
                    MinCostFlowGraph::<i32>::new(input.people_count + needed_barricades.len() + 2);
                let barricade_offset = input.people_count;
                let source = barricade_offset + needed_barricades.len();
                let sink = source + 1;

                // 人 -> バリケード
                for (i, &person) in input.people.iter().enumerate() {
                    if available[i] {
                        let distances = common::calculate_distances(barricade_map, person);

                        for (j, &barricade) in needed_barricades.iter().enumerate() {
                            let dist = distances[barricade.row][barricade.col];

                            if dist < std::i32::MAX {
                                graph.add_edge(i, j + barricade_offset, 1, dist);
                            } else {
                                graph.add_edge(i, j + barricade_offset, 0, 0);
                            }
                        }
                    } else {
                        for j in 0..needed_barricades.len() {
                            graph.add_edge(i, j + barricade_offset, 0, 0);
                        }
                    }
                }

                // source -> 人
                for i in 0..input.people_count {
                    graph.add_edge(source, i, 1, 0);
                }

                // バリケード -> sink
                const MINI_INF: i32 = 1_000_000;
                for j in 0..needed_barricades.len() {
                    graph.add_edge(j + barricade_offset, sink, 1, 0);
                    graph.add_edge(j + barricade_offset, sink, std::i32::MAX, MINI_INF);
                }

                graph.flow(source, sink, std::i32::MAX);

                let mut matchings = vec![None; input.people_count];

                for i in 0..input.people_count {
                    for j in 0..needed_barricades.len() {
                        let edge = graph.get_edge(i * needed_barricades.len() + j);
                        if edge.flow > 0 {
                            matchings[i] = Some(needed_barricades[j]);
                        }
                    }
                }

                matchings
            }
        }

        #[allow(dead_code)]
        mod common {
            use std::collections::VecDeque;

            use crate::{
                ahc008::{Input, MAP_SIZE},
                grid::{Coordinate, CoordinateDiff, ADJACENTS},
            };

            pub(super) fn calculate_all_distances(
                people: &[Coordinate],
                barricades: &[Vec<bool>],
            ) -> Vec<Vec<Vec<i32>>> {
                let mut distances = Vec::with_capacity(people.len());

                for &person in people.iter() {
                    distances.push(calculate_distances(barricades, person));
                }

                distances
            }

            /// startからの距離を求める
            pub(super) fn calculate_distances(
                barricades: &[Vec<bool>],
                start: Coordinate,
            ) -> Vec<Vec<i32>> {
                let mut distances = mat![std::i32::MAX; barricades.len(); barricades[0].len()];
                let mut queue = VecDeque::new();
                distances[start.row][start.col] = 0;
                queue.push_back(start);

                while let Some(current) = queue.pop_front() {
                    let dist = distances[current.row][current.col];

                    for &adj in ADJACENTS.iter() {
                        let next = current + adj;

                        if !barricades[next.row][next.col]
                            && chmin!(distances[next.row][next.col], dist + 1)
                        {
                            queue.push_back(next);
                        }
                    }
                }

                distances
            }

            pub(super) fn calculate_route(
                barricades: &[Vec<bool>],
                start: Coordinate,
                goal: Coordinate,
            ) -> Option<Vec<CoordinateDiff>> {
                let mut distances = mat![std::i32::MAX; barricades.len(); barricades[0].len()];
                let mut from =
                    mat![CoordinateDiff::new(0, 0); barricades.len(); barricades[0].len()];
                let mut queue = VecDeque::new();
                distances[start.row][start.col] = 0;
                queue.push_back(start);

                while let Some(current) = queue.pop_front() {
                    let dist = distances[current.row][current.col];

                    for &adj in ADJACENTS.iter() {
                        let next = current + adj;

                        if !barricades[next.row][next.col]
                            && chmin!(distances[next.row][next.col], dist + 1)
                        {
                            queue.push_back(next);
                            from[next.row][next.col] = adj;
                        }
                    }
                }

                if distances[goal.row][goal.col] == std::i32::MAX {
                    return None;
                }

                let mut route = vec![];
                let mut current = goal;

                while current != start {
                    let adj = from[current.row][current.col];
                    route.push(adj);
                    current += adj.invert();
                }

                route.reverse();

                Some(route)
            }

            pub(super) fn calculate_score(input: &Input) -> i32 {
                let (connection_map, connection_count) = get_connection_map(input);
                let mut people_counts = vec![0.0; connection_count];
                let mut pet_counts = vec![0; connection_count];
                let mut spaces = vec![0.0; connection_count];

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        let id = connection_map[row][col];

                        if id < spaces.len() {
                            spaces[id] += 1.0;
                        }
                    }
                }

                for person in input.people.iter() {
                    people_counts[connection_map[person.row][person.col]] += 1.0;
                }

                for pet in input.pets.iter() {
                    let c = pet.coordinate;
                    let id = connection_map[c.row][c.col];

                    if id < pet_counts.len() {
                        pet_counts[connection_map[c.row][c.col]] += 1;
                    }
                }

                let mut score = 0.0;

                for (&people, (&pets, &space)) in people_counts
                    .iter()
                    .zip(pet_counts.iter().zip(spaces.iter()))
                {
                    let s = people * space / 2.0f64.powi(pets);
                    score += s;
                }

                (1e8 / 900.0 * score / input.people_count as f64).round() as i32
            }

            pub(super) fn get_connection_map(input: &Input) -> (Vec<Vec<usize>>, usize) {
                const NONE: usize = !0;
                let barricade_map = input.get_barricade_map();
                let mut connection_map = mat![NONE; MAP_SIZE; MAP_SIZE];
                let mut stack = vec![];
                let mut count = 0;

                for row in 0..MAP_SIZE {
                    for col in 0..MAP_SIZE {
                        if barricade_map[row][col] || connection_map[row][col] != NONE {
                            continue;
                        }

                        connection_map[row][col] = count;
                        stack.push(Coordinate::new(row, col));

                        while let Some(c) = stack.pop() {
                            for &adj in ADJACENTS.iter() {
                                let next = c + adj;

                                if !barricade_map[next.row][next.col]
                                    && connection_map[next.row][next.col] == NONE
                                {
                                    connection_map[next.row][next.col] = count;
                                    stack.push(next);
                                }
                            }
                        }

                        count += 1;
                    }
                }

                (connection_map, count)
            }

            pub(super) fn get_majority_people_group(
                input: &Input,
                connection_map: &Vec<Vec<usize>>,
                group_count: usize,
            ) -> usize {
                let mut people_counts = vec![0; group_count];

                for person in input.people.iter() {
                    let group = connection_map[person.row][person.col];
                    if group < people_counts.len() {
                        people_counts[group] += 1;
                    }
                }

                let mut majority_group = 0;
                let mut majority_count = 0;

                for (i, &c) in people_counts.iter().enumerate() {
                    if chmax!(majority_count, c) {
                        majority_group = i;
                    }
                }

                majority_group
            }

            pub(super) fn get_alive_pet_indices(
                input: &Input,
                connection_map: &Vec<Vec<usize>>,
                majority_group: usize,
            ) -> Vec<usize> {
                let mut pets = vec![];

                for (i, pet) in input.pets.iter().enumerate() {
                    let c = pet.coordinate;

                    if connection_map[c.row][c.col] == majority_group {
                        pets.push(i);
                    }
                }

                pets
            }
        }
    }

    mod io {
        use self::official_judge::Sim;

        use super::*;
        use crate::grid::{ADJACENTS, DIRECTIONS};
        use text_io::read;

        pub(super) fn initialize() -> Input {
            fn read_coordinate() -> Coordinate {
                let row = read!();
                let col = read!();
                Coordinate::new(row, col)
            }

            let pet_count = read!();
            let mut pets = Vec::with_capacity(pet_count);
            let since = Instant::now();

            for _ in 0..pet_count {
                let coordinate = read_coordinate();
                let pet_type: usize = read!();
                let pet_type = match pet_type {
                    1 => PetType::Cow,
                    2 => PetType::Pig,
                    3 => PetType::Rabbit,
                    4 => PetType::Dog,
                    5 => PetType::Cat,
                    _ => unreachable!(),
                };

                let pet = Pet::new(coordinate, pet_type);
                pets.push(pet);
            }

            let people_count = read!();
            let mut people = Vec::with_capacity(people_count);

            for _ in 0..people_count {
                let coordinate = read_coordinate();
                people.push(coordinate);
            }

            let mut barricades = Vec::with_capacity((MAP_SIZE - 1) * 4);

            for row in 0..MAP_SIZE {
                for col in 0..MAP_SIZE {
                    if row == 0 || row == MAP_SIZE - 1 || col == 0 || col == MAP_SIZE - 1 {
                        barricades.push(Coordinate::new(row, col));
                    }
                }
            }

            let input = Input {
                pet_count,
                pets,
                people_count,
                people,
                barricades,
                since,
            };

            print_parameters(&input);
            input
        }

        fn print_parameters(input: &Input) {
            let mut counts = vec![0; 5];

            for pet in input.pets.iter() {
                let index = match pet.pet_type {
                    PetType::Cow => 0,
                    PetType::Pig => 1,
                    PetType::Rabbit => 2,
                    PetType::Dog => 3,
                    PetType::Cat => 4,
                };

                counts[index] += 1;
            }

            eprintln!(
                "parameters {},{},{},{},{},{}",
                input.people_count, counts[0], counts[1], counts[2], counts[3], counts[4]
            );
        }

        pub(super) trait Judge {
            fn write_commands(&mut self, commands: &[Command]);
            fn read_commands(&mut self, input: &Input) -> Vec<Vec<Command>>;
            fn calculate_score(&self, input: &Input) -> i32;
        }

        pub(super) struct RealJudge {}

        impl RealJudge {
            pub(super) fn new() -> Self {
                Self {}
            }
        }

        impl Judge for RealJudge {
            fn write_commands(&mut self, commands: &[Command]) {
                let mut output = String::new();

                'command: for &command in commands.iter() {
                    match command {
                        Command::None => output.push('.'),
                        Command::Put(diff) => {
                            for (&adj, &dir) in ADJACENTS.iter().zip(DIRECTIONS.iter()) {
                                if diff == adj {
                                    output.push(dir.to_ascii_lowercase());
                                    continue 'command;
                                }
                            }

                            unreachable!();
                        }
                        Command::Move(diff) => {
                            for (&adj, &dir) in ADJACENTS.iter().zip(DIRECTIONS.iter()) {
                                if diff == adj {
                                    output.push(dir.to_ascii_uppercase());
                                    continue 'command;
                                }
                            }

                            unreachable!();
                        }
                    }
                }

                println!("{}", output);
            }

            fn read_commands(&mut self, input: &Input) -> Vec<Vec<Command>> {
                let mut commands = Vec::with_capacity(input.pet_count);

                for _ in 0..input.pet_count {
                    let raw_commands: String = read!();
                    let mut command = vec![];

                    for c in raw_commands.chars() {
                        let adj = grid::char_to_adj(c);
                        command.push(Command::Move(adj));
                    }

                    commands.push(command);
                }

                commands
            }

            fn calculate_score(&self, input: &Input) -> i32 {
                let sim = official_judge::Sim::new(input, 0, 0);
                sim.compute_score()
            }
        }
        pub(super) struct VirtualJudge {
            sim: Sim,
        }

        impl VirtualJudge {
            pub(super) fn new(input: &crate::ahc008::Input, turn: usize, seed: u64) -> Self {
                let sim = Sim::new(input, turn, seed);
                Self { sim }
            }
        }

        impl Judge for VirtualJudge {
            fn write_commands(&mut self, commands: &[Command]) {
                self.sim.human_move(commands).unwrap();
            }

            fn read_commands(&mut self, _input: &Input) -> Vec<Vec<Command>> {
                self.sim.pet_move()
            }

            fn calculate_score(&self, _input: &Input) -> i32 {
                self.sim.compute_score()
            }
        }

        /// 公式ジャッジツールより一部改変
        mod official_judge {
            trait SetMinMax {
                fn setmin(&mut self, v: Self) -> bool;
                fn setmax(&mut self, v: Self) -> bool;
            }
            impl<T> SetMinMax for T
            where
                T: PartialOrd,
            {
                fn setmin(&mut self, v: T) -> bool {
                    *self > v && {
                        *self = v;
                        true
                    }
                }
                fn setmax(&mut self, v: T) -> bool {
                    *self < v && {
                        *self = v;
                        true
                    }
                }
            }

            const D: usize = 30;
            const DIR: [(usize, usize); 4] = [(!0, 0), (1, 0), (0, !0), (0, 1)];
            const DIR_CHAR: [char; 5] = ['U', 'D', 'L', 'R', '.'];

            use std::collections::VecDeque;

            use crate::{
                ahc008::Command,
                grid::{self, CoordinateDiff},
            };
            use rand::{prelude::SliceRandom, SeedableRng};

            fn bfs(blocked: &Vec<Vec<bool>>, sx: usize, sy: usize) -> Vec<Vec<usize>> {
                let mut dist = mat![!0; D; D];
                let mut que = VecDeque::new();
                que.push_back((sx, sy));
                dist[sx][sy] = 0;
                while let Some((x, y)) = que.pop_front() {
                    let t = dist[x][y];
                    for d in 0..4 {
                        let x2 = x + DIR[d].0;
                        let y2 = y + DIR[d].1;
                        if x2 < D && y2 < D && !blocked[x2][y2] && dist[x2][y2].setmin(t + 1) {
                            que.push_back((x2, y2));
                        }
                    }
                }
                dist
            }

            #[derive(Copy, Clone, Debug, PartialEq, Eq, PartialOrd, Ord)]
            pub(super) enum Pet {
                Cow,
                Pig,
                Rabbit,
                Dog(usize),
                Cat(usize, usize),
            }

            pub(super) struct Sim {
                pub blocked: Vec<Vec<bool>>,
                pub rng: rand_chacha::ChaCha20Rng,
                pub ps: Vec<(usize, usize, Pet)>,
                pub hs: Vec<(usize, usize)>,
                pub turn: usize,
            }

            impl Sim {
                pub(super) fn new(input: &crate::ahc008::Input, turn: usize, seed: u64) -> Self {
                    let mut blocked = mat![false; D; D];

                    for &barricade in input.barricades.iter() {
                        let barricade = barricade + CoordinateDiff::new(!0, !0);
                        if barricade.in_map(D) {
                            blocked[barricade.row][barricade.col] = true;
                        }
                    }

                    let ps = input
                        .pets
                        .iter()
                        .map(|p| {
                            let x = p.coordinate.row - 1;
                            let y = p.coordinate.col - 1;
                            let t = match p.pet_type {
                                crate::ahc008::PetType::Cow => Pet::Cow,
                                crate::ahc008::PetType::Pig => Pet::Pig,
                                crate::ahc008::PetType::Rabbit => Pet::Rabbit,
                                crate::ahc008::PetType::Dog => Pet::Dog(!0),
                                crate::ahc008::PetType::Cat => Pet::Cat(!0, !0),
                            };
                            (x, y, t)
                        })
                        .collect();

                    let hs = input
                        .people
                        .iter()
                        .map(|p| (p.row - 1, p.col - 1))
                        .collect();

                    Self {
                        blocked,
                        rng: rand_chacha::ChaCha20Rng::seed_from_u64(seed),
                        ps,
                        hs,
                        turn,
                    }
                }

                pub(super) fn human_move(&mut self, out: &[Command]) -> Result<(), String> {
                    self.turn += 1;
                    if out.len() != self.hs.len() {
                        return Err(format!("illegal output length (turn {})", self.turn));
                    }
                    let mut new_hs = self.hs.clone();
                    for (i, c) in out.iter().enumerate() {
                        match c {
                            &Command::None => (),
                            &Command::Put(dir) => {
                                let x = self.hs[i].0 + dir.dr;
                                let y = self.hs[i].1 + dir.dc;

                                if x >= D || y >= D {
                                    continue;
                                }
                                for j in 0..self.ps.len() {
                                    if self.ps[j].0 == x && self.ps[j].1 == y {
                                        return Err(format!(
                                            "trying to block a square containing a pet (turn {})",
                                            self.turn
                                        ));
                                    }
                                }
                                for j in 0..self.hs.len() {
                                    if self.hs[j].0 == x && self.hs[j].1 == y {
                                        return Err(format!(
                                            "trying to block a square containing a human (turn {})",
                                            self.turn
                                        ));
                                    }
                                }
                                for d in 0..4 {
                                    let x2 = x + DIR[d].0;
                                    let y2 = y + DIR[d].1;
                                    if x2 < D
                                        && y2 < D
                                        && self.ps.iter().any(|&(x, y, _)| (x2, y2) == (x, y))
                                    {
                                        return Err(format!("trying to block a square whose adjacent square contains a pet (turn {})", self.turn));
                                    }
                                }
                                self.blocked[x][y] = true;
                            }
                            &Command::Move(dir) => {
                                new_hs[i].0 += dir.dr;
                                new_hs[i].1 += dir.dc;
                                if new_hs[i].0 >= D || new_hs[i].1 >= D {
                                    return Err(format!(
                                        "trying to move to an impassible square (turn {})",
                                        self.turn
                                    ));
                                }
                            }
                        }
                    }
                    self.hs = new_hs;
                    for i in 0..self.hs.len() {
                        if self.blocked[self.hs[i].0][self.hs[i].1] {
                            return Err(format!(
                                "trying to move to an impassible square (turn {})",
                                self.turn
                            ));
                        }
                    }
                    Ok(())
                }

                pub(super) fn standard_move(&mut self, x: &mut usize, y: &mut usize) -> usize {
                    let mut cand = vec![];
                    for d in 0..4 {
                        let x2 = *x + DIR[d].0;
                        let y2 = *y + DIR[d].1;
                        if x2 < D && y2 < D && !self.blocked[x2][y2] {
                            cand.push(d);
                        }
                    }
                    let d = *cand.choose(&mut self.rng).unwrap();
                    *x += DIR[d].0;
                    *y += DIR[d].1;
                    d
                }

                pub(super) fn pet_move(&mut self) -> Vec<Vec<Command>> {
                    let mut ps = self.ps.clone();
                    let ret = ps
                        .iter_mut()
                        .map(|(x, y, p)| match p {
                            Pet::Cow => {
                                vec![DIR_CHAR[self.standard_move(x, y)]]
                            }
                            Pet::Pig => {
                                let mut ret = vec![];
                                for _ in 0..2 {
                                    ret.push(DIR_CHAR[self.standard_move(x, y)]);
                                }
                                ret
                            }
                            Pet::Rabbit => {
                                let mut ret = vec![];
                                for _ in 0..3 {
                                    ret.push(DIR_CHAR[self.standard_move(x, y)]);
                                }
                                ret
                            }
                            Pet::Dog(target) => loop {
                                if *target == !0 || self.hs[*target] == (*x, *y) {
                                    let dist = bfs(&self.blocked, *x, *y);
                                    let cand = (0..self.hs.len())
                                        .filter(|&i| {
                                            dist[self.hs[i].0][self.hs[i].1] != !0
                                                && dist[self.hs[i].0][self.hs[i].1] > 0
                                        })
                                        .collect::<Vec<_>>();
                                    if let Some(i) = cand.choose(&mut self.rng) {
                                        *target = *i;
                                    } else {
                                        *target = !0;
                                        return vec![DIR_CHAR[self.standard_move(x, y)]];
                                    }
                                }
                                let (tx, ty) = self.hs[*target];
                                let dist = bfs(&self.blocked, tx, ty);
                                if dist[*x][*y] == !0 {
                                    *target = !0;
                                    continue;
                                }
                                let mut cand = vec![];
                                for d in 0..4 {
                                    let x2 = *x + DIR[d].0;
                                    let y2 = *y + DIR[d].1;
                                    if x2 < D && y2 < D && dist[*x][*y] > dist[x2][y2] {
                                        cand.push(d);
                                    }
                                }
                                let dir = *cand.choose(&mut self.rng).unwrap();
                                *x += DIR[dir].0;
                                *y += DIR[dir].1;
                                if self.hs[*target] == (*x, *y) {
                                    *target = !0;
                                }
                                let ret = vec![DIR_CHAR[dir], DIR_CHAR[self.standard_move(x, y)]];
                                if *target != !0 && self.hs[*target] == (*x, *y) {
                                    *target = !0;
                                }
                                return ret;
                            },
                            Pet::Cat(tx, ty) => loop {
                                if *tx != !0 && self.blocked[*tx][*ty] {
                                    *tx = !0;
                                    *ty = !0;
                                }
                                if *tx == !0 {
                                    let dist = bfs(&self.blocked, *x, *y);
                                    let mut cand = vec![];
                                    for i in 0..D {
                                        for j in 0..D {
                                            if dist[i][j] != !0 && dist[i][j] > 0 {
                                                cand.push((i, j));
                                            }
                                        }
                                    }
                                    let (i, j) = *cand.choose(&mut self.rng).unwrap();
                                    *tx = i;
                                    *ty = j;
                                }
                                let dist = bfs(&self.blocked, *tx, *ty);
                                if dist[*x][*y] == !0 {
                                    *tx = !0;
                                    *ty = !0;
                                    continue;
                                }
                                let mut cand = vec![];
                                for d in 0..4 {
                                    let x2 = *x + DIR[d].0;
                                    let y2 = *y + DIR[d].1;
                                    if x2 < D && y2 < D && dist[*x][*y] > dist[x2][y2] {
                                        cand.push(d);
                                    }
                                }
                                let dir = *cand.choose(&mut self.rng).unwrap();
                                *x += DIR[dir].0;
                                *y += DIR[dir].1;
                                if (*tx, *ty) == (*x, *y) {
                                    *tx = !0;
                                    *ty = !0;
                                }
                                let ret = vec![DIR_CHAR[dir], DIR_CHAR[self.standard_move(x, y)]];
                                if (*tx, *ty) == (*x, *y) {
                                    *tx = !0;
                                    *ty = !0;
                                }
                                return ret;
                            },
                        })
                        .map(|s| {
                            s.iter()
                                .map(|&c| Command::Move(grid::char_to_adj(c)))
                                .collect::<Vec<_>>()
                        })
                        .collect();
                    self.ps = ps;
                    ret
                }

                pub(super) fn compute_score(&self) -> i32 {
                    let mut score = 0.0;
                    for &(x, y) in &self.hs {
                        let dist = bfs(&self.blocked, x, y);
                        let mut r = 0;
                        for i in 0..D {
                            for j in 0..D {
                                if dist[i][j] != !0 {
                                    r += 1;
                                }
                            }
                        }
                        let mut n = 0;
                        for &(px, py, _) in &self.ps {
                            if dist[px][py] != !0 {
                                n += 1;
                            }
                        }
                        score += r as f64 / (30.0 * 30.0) * 2.0f64.powi(-n);
                    }
                    (1e8 * score / self.hs.len() as f64).round() as i32
                }
            }
        }
    }

    mod base64 {
        pub(super) fn to_f64(data: &[u8]) -> Vec<f64> {
            const BASE64_MAP: &[u8] =
                b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            let mut stream = vec![];

            let mut cursor = 0;

            while cursor + 4 <= data.len() {
                let mut buffer = 0u32;

                for i in 0..4 {
                    let c = data[cursor + i];
                    let shift = 6 * (3 - i);

                    for (i, &d) in BASE64_MAP.iter().enumerate() {
                        if c == d {
                            buffer |= (i as u32) << shift;
                        }
                    }
                }

                for i in 0..3 {
                    let shift = 8 * (2 - i);
                    let value = (buffer >> shift) as u8;
                    stream.push(value);
                }

                cursor += 4;
            }

            let mut result = vec![];
            cursor = 0;

            while cursor + 8 <= stream.len() {
                let p = stream.as_ptr() as *const f64;
                let x = unsafe { *p.offset(cursor as isize / 8) };
                result.push(x);
                cursor += 8;
            }

            result
        }
    }

    mod neural_network {
        use crate::ahc008::base64;

        pub(super) struct NeuralNetwork {
            in_weight: Vec<Vec<f64>>,
            in_bias: Vec<f64>,
            hidden1_weight: Vec<Vec<f64>>,
            hidden1_bias: Vec<f64>,
            hidden2_weight: Vec<Vec<f64>>,
            hidden2_bias: Vec<f64>,
            hidden3_weight: Vec<Vec<f64>>,
            hidden3_bias: Vec<f64>,
            out_weight: Vec<Vec<f64>>,
            out_bias: Vec<f64>,
        }

        impl NeuralNetwork {
            pub(super) fn predict(&self, x: &[f64]) -> Vec<f64> {
                let x = multiply_matrix(&self.in_weight, x);
                let x = add_vector(&self.in_bias, &x);
                let x = apply(&x, relu);

                let x = multiply_matrix(&self.hidden1_weight, &x);
                let x = add_vector(&self.hidden1_bias, &x);
                let x = apply(&x, relu);

                let x = multiply_matrix(&self.hidden2_weight, &x);
                let x = add_vector(&self.hidden2_bias, &x);
                let x = apply(&x, relu);

                let x = multiply_matrix(&self.hidden3_weight, &x);
                let x = add_vector(&self.hidden3_bias, &x);
                let x = apply(&x, relu);

                let x = multiply_matrix(&self.out_weight, &x);
                let x = add_vector(&self.out_bias, &x);
                let x = apply(&x, sigmoid);

                x
            }
        }

        pub(super) fn generate_model() -> NeuralNetwork {
            const WEIGHT_BASE64: &[u8] = b"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";
            const INPUT_SIZE: usize = 7;
            const HIDDEN_SIZE: usize = 32;
            const OUTPUT_SIZE: usize = 5;
            let weight = base64::to_f64(WEIGHT_BASE64);
            let mut cursor = 0;

            let in_weight = read_matrix(HIDDEN_SIZE, INPUT_SIZE, &weight, &mut cursor);
            let in_bias = read_vector(HIDDEN_SIZE, &weight, &mut cursor);
            let hidden1_weight = read_matrix(HIDDEN_SIZE, HIDDEN_SIZE, &weight, &mut cursor);
            let hidden1_bias = read_vector(HIDDEN_SIZE, &weight, &mut cursor);
            let hidden2_weight = read_matrix(HIDDEN_SIZE, HIDDEN_SIZE, &weight, &mut cursor);
            let hidden2_bias = read_vector(HIDDEN_SIZE, &weight, &mut cursor);
            let hidden3_weight = read_matrix(HIDDEN_SIZE, HIDDEN_SIZE, &weight, &mut cursor);
            let hidden3_bias = read_vector(HIDDEN_SIZE, &weight, &mut cursor);
            let out_weight = read_matrix(OUTPUT_SIZE, HIDDEN_SIZE, &weight, &mut cursor);
            let out_bias = read_vector(OUTPUT_SIZE, &weight, &mut cursor);

            NeuralNetwork {
                in_weight,
                in_bias,
                hidden1_weight,
                hidden1_bias,
                hidden2_weight,
                hidden2_bias,
                hidden3_weight,
                hidden3_bias,
                out_weight,
                out_bias,
            }
        }

        fn read_matrix(row: usize, col: usize, data: &[f64], cursor: &mut usize) -> Vec<Vec<f64>> {
            let mut matrix = Vec::with_capacity(row);

            for _ in 0..row {
                let vector = read_vector(col, data, cursor);
                matrix.push(vector);
            }

            matrix
        }

        fn read_vector(n: usize, data: &[f64], cursor: &mut usize) -> Vec<f64> {
            let mut vector = Vec::with_capacity(n);

            for _ in 0..n {
                vector.push(data[*cursor]);
                *cursor += 1;
            }

            vector
        }

        fn multiply_matrix(matrix: &[Vec<f64>], vector: &[f64]) -> Vec<f64> {
            let mut result = Vec::with_capacity(matrix.len());

            for line in matrix.iter() {
                debug_assert!(line.len() == vector.len());
                let mut sum = 0.0;

                for (x, y) in line.iter().zip(vector.iter()) {
                    sum += x * y;
                }

                result.push(sum);
            }

            result
        }

        fn add_vector(vector1: &[f64], vector2: &[f64]) -> Vec<f64> {
            let mut result = Vec::with_capacity(vector1.len());
            debug_assert!(vector1.len() == vector2.len());

            for (x, y) in vector1.iter().zip(vector2.iter()) {
                result.push(x + y);
            }

            result
        }

        fn apply<F>(vector: &[f64], f: F) -> Vec<f64>
        where
            F: Fn(f64) -> f64,
        {
            let mut result = Vec::with_capacity(vector.len());

            for x in vector.iter() {
                result.push(f(*x));
            }

            result
        }

        fn relu(x: f64) -> f64 {
            x.max(0.0)
        }

        fn sigmoid(x: f64) -> f64 {
            1.0 / (1.0 + f64::exp(-x))
        }
    }
}

#[allow(dead_code)]
mod grid {
    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
    pub struct Coordinate {
        pub row: usize,
        pub col: usize,
    }

    impl Coordinate {
        pub const fn new(row: usize, col: usize) -> Self {
            Self { row, col }
        }

        pub fn in_map(&self, size: usize) -> bool {
            self.row < size && self.col < size
        }

        pub const fn to_index(&self, size: usize) -> usize {
            self.row * size + self.col
        }

        pub fn distance_to(&self, other: Coordinate) -> i32 {
            (self.row as i32 - other.row as i32).abs() + (self.col as i32 - other.col as i32).abs()
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
    pub struct CoordinateDiff {
        pub dr: usize,
        pub dc: usize,
    }

    impl CoordinateDiff {
        pub const fn new(dr: usize, dc: usize) -> Self {
            Self { dr, dc }
        }

        pub const fn invert(&self) -> Self {
            Self::new(0usize.wrapping_sub(self.dr), 0usize.wrapping_sub(self.dc))
        }
    }

    impl std::ops::Add<CoordinateDiff> for Coordinate {
        type Output = Coordinate;

        fn add(self, rhs: CoordinateDiff) -> Self::Output {
            Coordinate::new(self.row.wrapping_add(rhs.dr), self.col.wrapping_add(rhs.dc))
        }
    }

    impl std::ops::AddAssign<CoordinateDiff> for Coordinate {
        fn add_assign(&mut self, rhs: CoordinateDiff) {
            self.row = self.row.wrapping_add(rhs.dr);
            self.col = self.col.wrapping_add(rhs.dc);
        }
    }

    impl std::ops::Mul<usize> for CoordinateDiff {
        type Output = CoordinateDiff;

        fn mul(self, rhs: usize) -> Self::Output {
            CoordinateDiff::new(self.dr.wrapping_mul(rhs), self.dc.wrapping_mul(rhs))
        }
    }

    pub const ADJACENTS: [CoordinateDiff; 4] = [
        CoordinateDiff::new(!0, 0),
        CoordinateDiff::new(0, 1),
        CoordinateDiff::new(1, 0),
        CoordinateDiff::new(0, !0),
    ];

    pub const VERTICAL_ADJACENTS: [CoordinateDiff; 2] =
        [CoordinateDiff::new(!0, 0), CoordinateDiff::new(1, 0)];

    pub const HORIZONTAL_ADJACENTS: [CoordinateDiff; 2] =
        [CoordinateDiff::new(0, 1), CoordinateDiff::new(0, !0)];

    pub const DIRECTIONS: [char; 4] = ['U', 'R', 'D', 'L'];

    pub fn char_to_adj(c: char) -> CoordinateDiff {
        ADJACENTS[char_to_dir(c)]
    }

    pub fn char_to_dir(c: char) -> usize {
        match c {
            'U' => 0,
            'R' => 1,
            'D' => 2,
            'L' => 3,
            _ => unreachable!(),
        }
    }
}

#[allow(dead_code)]
mod acl {
    mod internal_queue {
        #![allow(dead_code)]

        #[derive(Default)]
        pub(crate) struct SimpleQueue<T> {
            payload: Vec<T>,
            pos: usize,
        }

        impl<T> SimpleQueue<T> {
            pub(crate) fn reserve(&mut self, n: usize) {
                if n > self.payload.len() {
                    self.payload.reserve(n - self.payload.len());
                }
            }

            pub(crate) fn size(&self) -> usize {
                self.payload.len() - self.pos
            }

            pub(crate) fn empty(&self) -> bool {
                self.pos == self.payload.len()
            }

            pub(crate) fn push(&mut self, t: T) {
                self.payload.push(t);
            }

            // Do we need mutable version?
            pub(crate) fn front(&self) -> Option<&T> {
                if self.pos < self.payload.len() {
                    Some(&self.payload[self.pos])
                } else {
                    None
                }
            }

            pub(crate) fn clear(&mut self) {
                self.payload.clear();
                self.pos = 0;
            }

            pub(crate) fn pop(&mut self) -> Option<&T> {
                if self.pos < self.payload.len() {
                    self.pos += 1;
                    Some(&self.payload[self.pos - 1])
                } else {
                    None
                }
            }
        }
    }

    mod internal_type_traits {
        use std::{
            fmt,
            iter::{Product, Sum},
            ops::{
                Add, AddAssign, BitAnd, BitAndAssign, BitOr, BitOrAssign, BitXor, BitXorAssign,
                Div, DivAssign, Mul, MulAssign, Not, Rem, RemAssign, Shl, ShlAssign, Shr,
                ShrAssign, Sub, SubAssign,
            },
        };

        // Skipped:
        //
        // - `is_signed_int_t<T>`   (probably won't be used directly in `modint.rs`)
        // - `is_unsigned_int_t<T>` (probably won't be used directly in `modint.rs`)
        // - `to_unsigned_t<T>`     (not used in `fenwicktree.rs`)

        /// Corresponds to `std::is_integral` in C++.
        // We will remove unnecessary bounds later.
        //
        // Maybe we should rename this to `PrimitiveInteger` or something, as it probably won't be used in the
        // same way as the original ACL.
        pub trait Integral:
            'static
            + Send
            + Sync
            + Copy
            + Ord
            + Not<Output = Self>
            + Add<Output = Self>
            + Sub<Output = Self>
            + Mul<Output = Self>
            + Div<Output = Self>
            + Rem<Output = Self>
            + AddAssign
            + SubAssign
            + MulAssign
            + DivAssign
            + RemAssign
            + Sum
            + Product
            + BitOr<Output = Self>
            + BitAnd<Output = Self>
            + BitXor<Output = Self>
            + BitOrAssign
            + BitAndAssign
            + BitXorAssign
            + Shl<Output = Self>
            + Shr<Output = Self>
            + ShlAssign
            + ShrAssign
            + fmt::Display
            + fmt::Debug
            + fmt::Binary
            + fmt::Octal
            + Zero
            + One
            + BoundedBelow
            + BoundedAbove
        {
        }

        /// Class that has additive identity element
        pub trait Zero {
            /// The additive identity element
            fn zero() -> Self;
        }

        /// Class that has multiplicative identity element
        pub trait One {
            /// The multiplicative identity element
            fn one() -> Self;
        }

        pub trait BoundedBelow {
            fn min_value() -> Self;
        }

        pub trait BoundedAbove {
            fn max_value() -> Self;
        }

        macro_rules! impl_integral {
            ($($ty:ty),*) => {
                $(
                    impl Zero for $ty {
                        #[inline]
                        fn zero() -> Self {
                            0
                        }
                    }

                    impl One for $ty {
                        #[inline]
                        fn one() -> Self {
                            1
                        }
                    }

                    impl BoundedBelow for $ty {
                        #[inline]
                        fn min_value() -> Self {
                            Self::min_value()
                        }
                    }

                    impl BoundedAbove for $ty {
                        #[inline]
                        fn max_value() -> Self {
                            Self::max_value()
                        }
                    }

                    impl Integral for $ty {}
                )*
            };
        }

        impl_integral!(i8, i16, i32, i64, i128, isize, u8, u16, u32, u64, u128, usize);
    }

    pub(crate) mod maxflow {
        #![allow(dead_code)]
        use super::internal_queue::SimpleQueue;
        use super::internal_type_traits::Integral;
        use std::cmp::min;
        use std::iter;

        impl<Cap> MfGraph<Cap>
        where
            Cap: Integral,
        {
            pub fn new(n: usize) -> MfGraph<Cap> {
                MfGraph {
                    _n: n,
                    pos: Vec::new(),
                    g: iter::repeat_with(Vec::new).take(n).collect(),
                }
            }

            pub fn add_edge(&mut self, from: usize, to: usize, cap: Cap) -> usize {
                assert!(from < self._n);
                assert!(to < self._n);
                assert!(Cap::zero() <= cap);
                let m = self.pos.len();
                self.pos.push((from, self.g[from].len()));
                let rev = self.g[to].len() + if from == to { 1 } else { 0 };
                self.g[from].push(_Edge { to, rev, cap });
                let rev = self.g[from].len() - 1;
                self.g[to].push(_Edge {
                    to: from,
                    rev,
                    cap: Cap::zero(),
                });
                m
            }
        }

        #[derive(Debug, PartialEq, Eq)]
        pub struct Edge<Cap: Integral> {
            pub from: usize,
            pub to: usize,
            pub cap: Cap,
            pub flow: Cap,
        }

        impl<Cap> MfGraph<Cap>
        where
            Cap: Integral,
        {
            pub fn get_edge(&self, i: usize) -> Edge<Cap> {
                let m = self.pos.len();
                assert!(i < m);
                let _e = &self.g[self.pos[i].0][self.pos[i].1];
                let _re = &self.g[_e.to][_e.rev];
                Edge {
                    from: self.pos[i].0,
                    to: _e.to,
                    cap: _e.cap + _re.cap,
                    flow: _re.cap,
                }
            }
            pub fn edges(&self) -> Vec<Edge<Cap>> {
                let m = self.pos.len();
                (0..m).map(|i| self.get_edge(i)).collect()
            }
            pub fn change_edge(&mut self, i: usize, new_cap: Cap, new_flow: Cap) {
                let m = self.pos.len();
                assert!(i < m);
                assert!(Cap::zero() <= new_flow && new_flow <= new_cap);
                let (to, rev) = {
                    let _e = &mut self.g[self.pos[i].0][self.pos[i].1];
                    _e.cap = new_cap - new_flow;
                    (_e.to, _e.rev)
                };
                let _re = &mut self.g[to][rev];
                _re.cap = new_flow;
            }

            /// `s != t` must hold, otherwise it panics.
            pub fn flow(&mut self, s: usize, t: usize) -> Cap {
                self.flow_with_capacity(s, t, Cap::max_value())
            }
            /// # Parameters
            /// * `s != t` must hold, otherwise it panics.
            /// * `flow_limit >= 0`
            pub fn flow_with_capacity(&mut self, s: usize, t: usize, flow_limit: Cap) -> Cap {
                let n_ = self._n;
                assert!(s < n_);
                assert!(t < n_);
                // By the definition of max flow in appendix.html, this function should return 0
                // when the same vertices are provided.  On the other hand, it is reasonable to
                // return infinity-like value too, which is what the original implementation
                // (and this implementation without the following assertion) does.
                // Since either return value is confusing, we'd rather deny the parameters
                // of the two same vertices.
                // For more details, see https://github.com/rust-lang-ja/ac-library-rs/pull/24#discussion_r485343451
                // and https://github.com/atcoder/ac-library/issues/5 .
                assert_ne!(s, t);
                // Additional constraint
                assert!(Cap::zero() <= flow_limit);

                let mut calc = FlowCalculator {
                    graph: self,
                    s,
                    t,
                    flow_limit,
                    level: vec![0; n_],
                    iter: vec![0; n_],
                    que: SimpleQueue::default(),
                };

                let mut flow = Cap::zero();
                while flow < flow_limit {
                    calc.bfs();
                    if calc.level[t] == -1 {
                        break;
                    }
                    calc.iter.iter_mut().for_each(|e| *e = 0);
                    while flow < flow_limit {
                        let f = calc.dfs(t, flow_limit - flow);
                        if f == Cap::zero() {
                            break;
                        }
                        flow += f;
                    }
                }
                flow
            }

            pub fn min_cut(&self, s: usize) -> Vec<bool> {
                let mut visited = vec![false; self._n];
                let mut que = SimpleQueue::default();
                que.push(s);
                while let Some(&p) = que.pop() {
                    visited[p] = true;
                    for e in &self.g[p] {
                        if e.cap != Cap::zero() && !visited[e.to] {
                            visited[e.to] = true;
                            que.push(e.to);
                        }
                    }
                }
                visited
            }
        }

        struct FlowCalculator<'a, Cap> {
            graph: &'a mut MfGraph<Cap>,
            s: usize,
            t: usize,
            flow_limit: Cap,
            level: Vec<i32>,
            iter: Vec<usize>,
            que: SimpleQueue<usize>,
        }

        impl<Cap> FlowCalculator<'_, Cap>
        where
            Cap: Integral,
        {
            fn bfs(&mut self) {
                self.level.iter_mut().for_each(|e| *e = -1);
                self.level[self.s] = 0;
                self.que.clear();
                self.que.push(self.s);
                while !self.que.empty() {
                    let v = *self.que.front().unwrap();
                    self.que.pop();
                    for e in &self.graph.g[v] {
                        if e.cap == Cap::zero() || self.level[e.to] >= 0 {
                            continue;
                        }
                        self.level[e.to] = self.level[v] + 1;
                        if e.to == self.t {
                            return;
                        }
                        self.que.push(e.to);
                    }
                }
            }
            fn dfs(&mut self, v: usize, up: Cap) -> Cap {
                if v == self.s {
                    return up;
                }
                let mut res = Cap::zero();
                let level_v = self.level[v];
                for i in self.iter[v]..self.graph.g[v].len() {
                    self.iter[v] = i;
                    let &_Edge {
                        to: e_to,
                        rev: e_rev,
                        ..
                    } = &self.graph.g[v][i];
                    if level_v <= self.level[e_to] || self.graph.g[e_to][e_rev].cap == Cap::zero() {
                        continue;
                    }
                    let d = self.dfs(e_to, min(up - res, self.graph.g[e_to][e_rev].cap));
                    if d <= Cap::zero() {
                        continue;
                    }
                    self.graph.g[v][i].cap += d;
                    self.graph.g[e_to][e_rev].cap -= d;
                    res += d;
                    if res == up {
                        return res;
                    }
                }
                self.iter[v] = self.graph.g[v].len();
                res
            }
        }

        #[derive(Default)]
        pub struct MfGraph<Cap> {
            _n: usize,
            pos: Vec<(usize, usize)>,
            g: Vec<Vec<_Edge<Cap>>>,
        }

        struct _Edge<Cap> {
            to: usize,
            rev: usize,
            cap: Cap,
        }
    }

    pub(crate) mod mincostflow {
        use super::internal_type_traits::Integral;

        pub struct MinCostFlowEdge<T> {
            pub from: usize,
            pub to: usize,
            pub cap: T,
            pub flow: T,
            pub cost: T,
        }

        pub struct MinCostFlowGraph<T> {
            pos: Vec<(usize, usize)>,
            g: Vec<Vec<_Edge<T>>>,
        }

        impl<T> MinCostFlowGraph<T>
        where
            T: Integral + std::ops::Neg<Output = T>,
        {
            pub fn new(n: usize) -> Self {
                Self {
                    pos: vec![],
                    g: (0..n).map(|_| vec![]).collect(),
                }
            }

            pub fn get_edge(&self, i: usize) -> MinCostFlowEdge<T> {
                assert!(i < self.pos.len());
                let e = &self.g[self.pos[i].0][self.pos[i].1];
                let re = &self.g[e.to][e.rev];
                MinCostFlowEdge {
                    from: self.pos[i].0,
                    to: e.to,
                    cap: e.cap + re.cap,
                    flow: re.cap,
                    cost: e.cost,
                }
            }

            pub fn edges(&self) -> Vec<MinCostFlowEdge<T>> {
                let m = self.pos.len();
                let mut result = vec![];
                for i in 0..m {
                    result.push(self.get_edge(i));
                }
                result
            }

            pub fn add_edge(&mut self, from: usize, to: usize, cap: T, cost: T) -> usize {
                assert!(from < self.g.len());
                assert!(to < self.g.len());
                assert_ne!(from, to);
                assert!(cap >= T::zero());
                assert!(cost >= T::zero());

                self.pos.push((from, self.g[from].len()));

                let rev = self.g[to].len();
                self.g[from].push(_Edge { to, rev, cap, cost });

                let rev = self.g[from].len() - 1;
                self.g[to].push(_Edge {
                    to: from,
                    rev,
                    cap: T::zero(),
                    cost: -cost,
                });

                self.pos.len() - 1
            }

            /// Returns (maximum flow, cost)
            pub fn flow(&mut self, source: usize, sink: usize, flow_limit: T) -> (T, T) {
                self.slope(source, sink, flow_limit).pop().unwrap()
            }

            pub fn slope(&mut self, source: usize, sink: usize, flow_limit: T) -> Vec<(T, T)> {
                let n = self.g.len();
                assert!(source < n);
                assert!(sink < n);
                assert_ne!(source, sink);

                let mut dual = vec![T::zero(); n];
                let mut prev_v = vec![0; n];
                let mut prev_e = vec![0; n];
                let mut flow = T::zero();
                let mut cost = T::zero();
                let mut prev_cost_per_flow: Option<T> = None;
                let mut result = vec![(flow, cost)];
                while flow < flow_limit {
                    if !self.refine_dual(source, sink, &mut dual, &mut prev_v, &mut prev_e) {
                        break;
                    }

                    let mut c = flow_limit - flow;
                    let mut v = sink;
                    while v != source {
                        c = std::cmp::min(c, self.g[prev_v[v]][prev_e[v]].cap);
                        v = prev_v[v];
                    }

                    let mut v = sink;
                    while v != source {
                        self.g[prev_v[v]][prev_e[v]].cap -= c;
                        let rev = self.g[prev_v[v]][prev_e[v]].rev;
                        self.g[v][rev].cap += c;
                        v = prev_v[v];
                    }

                    let d = -dual[source];
                    flow += c;
                    cost += d * c;
                    if prev_cost_per_flow == Some(d) {
                        assert!(result.pop().is_some());
                    }
                    result.push((flow, cost));
                    prev_cost_per_flow = Some(d);
                }
                result
            }

            fn refine_dual(
                &self,
                source: usize,
                sink: usize,
                dual: &mut [T],
                pv: &mut [usize],
                pe: &mut [usize],
            ) -> bool {
                let n = self.g.len();
                let mut dist = vec![T::max_value(); n];
                let mut vis = vec![false; n];

                let mut que = std::collections::BinaryHeap::new();
                dist[source] = T::zero();
                que.push((std::cmp::Reverse(T::zero()), source));
                while let Some((_, v)) = que.pop() {
                    if vis[v] {
                        continue;
                    }
                    vis[v] = true;
                    if v == sink {
                        break;
                    }

                    for (i, e) in self.g[v].iter().enumerate() {
                        if vis[e.to] || e.cap == T::zero() {
                            continue;
                        }

                        let cost = e.cost - dual[e.to] + dual[v];
                        if dist[e.to] - dist[v] > cost {
                            dist[e.to] = dist[v] + cost;
                            pv[e.to] = v;
                            pe[e.to] = i;
                            que.push((std::cmp::Reverse(dist[e.to]), e.to));
                        }
                    }
                }

                if !vis[sink] {
                    return false;
                }

                for v in 0..n {
                    if !vis[v] {
                        continue;
                    }
                    dual[v] -= dist[sink] - dist[v];
                }
                true
            }
        }

        struct _Edge<T> {
            to: usize,
            rev: usize,
            cap: T,
            cost: T,
        }
    }
}
