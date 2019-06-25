package superstring

import scala.annotation.tailrec
import scala.language.postfixOps
import scala.collection.parallel.immutable._
import superstring.Genetic.MappedString



object Solver {
  private val POPULATION = 25
  private val GENERATIONS = 10000
  private val MUTATION_LIMIT = POPULATION / 2
  private val MUTATION_PROBABILITY = 25
  private val FRESH_BLOOD_PROBABILITY = MUTATION_PROBABILITY / 2

  private val r = scala.util.Random

  type Genome = Vector[Int]
  type Horde = ParVector[Genome]

  case class RankedGenome(genome: Genome, rank: Int)

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

  private def prefixForPair(left: MappedString, right: String): Int = {
    val suffixMap = left.suffixMap.getOrElse(right.head, Seq.empty[Int])
    prefixLengths(left.str, right, suffixMap, Seq.empty[Int]).reduceOption(_ min _).getOrElse(0)
  }

  private def fitness(genome: Genome, strings: Vector[MappedString]): Int =
    genome.sliding(2).toVector.map(pair => {
      val left = strings(pair.head)
      val right = strings(pair.last)
      left.str.length + right.str.length - 2 * prefixForPair(left, right.str)
    }).sum

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

  def genomeToString(genome: Genome, strings: Vector[MappedString]): String = {
    val result = new StringBuilder
    result.append(strings(genome.head).str)
    genome.sliding(2).toVector
      .map(pair => (strings(pair(1)).str, prefixForPair(strings(pair(0)), strings(pair(1)).str)))
      .map(strWithPrefix => strWithPrefix._1.slice(strWithPrefix._2, strWithPrefix._1.length))
      .foreach(result.append)
    result.toString()
  }

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
    System.out.print(s"\rGeneration: $generation, top score pair is ${winners.head.rank}, ${winners.tail.head.rank}, string length is ${bestString.length}")
    evolve(newHorde, strings, template, generation - 1)
  }

  def Solve(strings: Vector[MappedString]): Unit = {
    val genomeTemplate = 0 until strings.size toVector

    val horde = new ParVector[Vector[Int]]((1 to POPULATION).map(_ => r.shuffle(genomeTemplate)).toVector)

    val bestString = evolve(horde, strings, genomeTemplate, GENERATIONS)
    System.out.println()
    System.out.println(s"Best string is $bestString")
    System.out.println(s"Best string length is ${bestString.length}")
  }
}
