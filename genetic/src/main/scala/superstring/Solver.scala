package superstring

import scala.annotation.tailrec
import scala.language.postfixOps
import scala.collection.parallel.immutable._
import superstring.Genetic.MappedString

import scala.collection.concurrent.TrieMap



object Solver {
  type Genome = Vector[Int]
  type Horde = ParVector[Genome]

  case class RankedGenome(genome: Genome, rank: Int)

  private val POPULATION = 250
  private val GENERATIONS = 20000
  private val MUTATION_LIMIT = POPULATION / 2
  private val MUTATION_PROBABILITY = 25
  private val FRESH_BLOOD_PROBABILITY = MUTATION_PROBABILITY / 2

  private val r = scala.util.Random
  private val pairCache = new TrieMap[(Int, Int), Int]()

  /**
    * Swaps two vector elements, specified by their indices.
    *
    * @param v     vector to operate on
    * @param left  index of the first element to swap
    * @param right index of the second elemend to swap
    * @return v with value of second element at the place of first element and vice versa
    */
  private def swapVectorElements[T](v: Vector[T], left: Int, right: Int): Vector[T] = {
    val leftVal = v(left)
    val rightVal = v(right)
    v.updated(left, rightVal).updated(right, leftVal)
  }

  /**
    * Finds lengths of a common prefix-suffix for pairs of strings
    * @param left 'suffix' string
    * @param right 'prefix' string
    * @param suffixes map with suffixes for a 'suffix' string
    * @param lengths sequence of lenghts of already found suffixes
    * @return sequence of possible common prefixes-suffixes
    */
  @tailrec
  private def prefixLengths(left: String, right: String, suffixes: Seq[Int], lengths: Seq[Int]): Seq[Int] = {
    if (suffixes.isEmpty) {
      return lengths
    }
    val suffixPos = suffixes.head
    val suffix = left.slice(suffixPos, left.length)
    val newSuffixes = if (right.startsWith(suffix)) {
      lengths :+ left.length - suffixPos
    } else {
      lengths
    }
    prefixLengths(left, right, suffixes.tail, newSuffixes)
  }

  /**
    * Selects a best prefixx-suffix for a pair
    * @param left 'suffix' string
    * @param right 'prefix' string
    * @return length of a best common prefix-suffix
    */
  private def prefixForPair(left: MappedString, right: String): Int = {
    val suffixMap = left.suffixMap.getOrElse(right.head, Seq.empty[Int])
    prefixLengths(left.str, right, suffixMap, Seq.empty[Int]).reduceOption(_ min _).getOrElse(0)
  }

  /**
    * Genetic algorithm fitness function. Calculates cost score for a genome, by
    * estimating resulting string length
    * @param genome Genome to process
    * @param strings Input strings with their suffixes.
    * @return genome rank
    */
  def fitness(genome: Genome, strings: Vector[MappedString]): Int =
    genome.sliding(2).toVector.map(pair => pairCache.getOrElseUpdate((pair.head, pair.last), {
      val left = strings(pair.head)
      val right = strings(pair.last)
      left.str.length + right.str.length - prefixForPair(left, right.str)
    })).sum

  /**
    *
    * Produces two offpsrings from two random parents.
    * For a first parent a random gene (index) is selected and
    * it's value used to find same gene (index) in the second parent.
    * Then we swap elements at genes positions in both parents
    * thus producing two offsprings.
    *
    * @param horde   current population
    * @param matings number of matings to make
    * @return population with additional offsprings
    */
  @tailrec
  def breed(horde: Horde, matings: Int): Horde = {
    if (matings == 0) {
      return horde
    }
    val firstParent = r.between(0, horde.length)
    val secondParent = r.between(0, horde.length)

    val geneFirst = r.between(0, horde(firstParent).length)

    val geneSecond = horde(secondParent) indexOf horde(firstParent)(geneFirst)

    val offSpringFirst = swapVectorElements(horde(firstParent), geneFirst, geneSecond)
    val offSpringSecond = swapVectorElements(horde(secondParent), geneFirst, geneSecond)

    val nextGeneration = horde ++ ParVector(offSpringFirst, offSpringSecond)

    breed(nextGeneration, matings - 1)
  }

  /**
    * Mutates a horde randomly.
    * @param horde Horde to process
    * @param mutations NUmber of mutations left
    * @return Horde with some mutated genomes
    */
  @tailrec
  def mutate(horde: Horde, mutations: Int): Horde = {
    if (mutations == 0) {
      return horde
    }
    val mutatedHorde = if (r.between(1, 100) < MUTATION_PROBABILITY) {
      val mutantIndex = r.between(0, horde.length)

      val mutant = horde(mutantIndex)

      val firstGene = r.between(0, mutant.length)
      val secondGene = r.between(0, mutant.length)

      horde.updated(mutantIndex, swapVectorElements(mutant, firstGene, secondGene))
    } else {
      horde
    }
    mutate(mutatedHorde, mutations - 1)
  }

  /**
    * Evicts genomes from the horde by randomly choosing a pair and removing
    * genome with a highest rank. Stop when horde size reaches desired population size.
    * @param horde Horde to work on
    * @return Horde with only the best genomes
    */
  @tailrec
  def tourney(horde: ParVector[RankedGenome]): ParVector[RankedGenome] = {
    if (horde.size == POPULATION) {
      return horde
    }

    val firstParty = r.between(0, horde.length)
    val secondParty = r.between(0, horde.length)

    if (horde(firstParty).rank > horde(secondParty).rank) {
      tourney(horde.take(firstParty) ++ horde.drop(firstParty + 1))
    } else {
      tourney(horde.take(secondParty) ++ horde.drop(secondParty + 1))
    }
  }

  /**
    * Helper function which converts genome to the actual string.
    * @param genome Genome to operate on
    * @param strings Input string array
    * @return string decoded from genome
    */
  def genomeToString(genome: Genome, strings: Vector[MappedString]): String = {
    val result = new StringBuilder
    result.append(strings(genome.head).str)
    genome.sliding(2).toVector
      .map(pair => (strings(pair(1)).str, prefixForPair(strings(pair(0)), strings(pair(1)).str)))
      .map(strWithPrefix => strWithPrefix._1.slice(strWithPrefix._2, strWithPrefix._1.length))
      .foreach(result.append)
    result.toString()
  }

  /**
    * Tries to find best superstring by messing with current variants and dropping worst ones
    * @param horde A horde to operate on
    * @param strings Array of input strings
    * @param template Template of a single genom
    * @param generation Number of current generation
    * @return The bext found superstring
    */
  @tailrec
  def evolve(horde: Horde, strings: Vector[MappedString], template: Genome, generation: Int): String = {
    if (generation == 0) {
      return genomeToString(horde.head, strings)
    }

    //Make next population
    val nextGeneration = breed(horde, POPULATION / 2)
    val mutatedGeneration = mutate(nextGeneration, MUTATION_LIMIT)
    val updatedGeneration = if (r.between(1, 100) < FRESH_BLOOD_PROBABILITY) {
      mutatedGeneration :+ r.shuffle(template)
    } else mutatedGeneration

    //Apply fitness function to the population
    val ranked = updatedGeneration
      .map(genome => RankedGenome(genome, fitness(genome, strings)))

    //Select best genomes
    val winners = tourney(ranked).toVector.sortBy(_.rank)

    val newHorde = new ParVector[Genome](winners.map(_.genome))

    val bestString = genomeToString(newHorde.head, strings)
    System.out.print(s"\rGeneration: ${GENERATIONS - generation}, top score pair is ${winners.head.rank}, ${winners.tail.head.rank}, string length is ${bestString.length}\t\t\t")
    evolve(newHorde, strings, template, generation - 1)
  }

  /**
    * Finds the best superstring for specified array of strings.
    * @param strings Strings with maps of their suffixes
    */
  def Solve(strings: Vector[MappedString]): Unit = {
    val genomeTemplate = 0 until strings.size toVector

    val horde = new ParVector[Vector[Int]]((1 to POPULATION).map(_ => r.shuffle(genomeTemplate)).toVector)

    val bestString = evolve(horde, strings, genomeTemplate, GENERATIONS)
    System.out.println()
    //System.out.println(s"Best string is $bestString")
    System.out.println(s"Best string length is ${bestString.length}")
  }
}
