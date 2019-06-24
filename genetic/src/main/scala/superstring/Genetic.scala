package superstring

import collection.parallel._

object Genetic {
  type SuffixMap = Map[Char, Seq[Int]]
  case class MappedString(str: String, suffixMap: SuffixMap)

  def buildSuffixMap(str: String): SuffixMap =
    str.zipWithIndex.foldLeft(Map[Char, Seq[Int]]())((sm, ci) => {
      val (char, index) = ci
      sm + (char -> sm.getOrElse(char, Seq[Int]()).:+(index))
    })

  def main(args: Array[String]): Unit = {
    if (args.isEmpty) {
      System.err.println("Usage: genetic <file_with_strings>")
      return
    }

    // 0 - Load the file
    val bufferedSource = io.Source.fromFile(args.head)
    val inputStrings = (for (line <- bufferedSource.getLines()) yield line).toList.distinct
    bufferedSource.close()
    System.out.println(s"Loaded ${inputStrings.size} words")

    //1 - Drop shorter words which are contained in a longer ones
    System.out.println("Dropping contained words")
    val nonContainedStrings = inputStrings.toParArray.filterNot(outer => inputStrings.exists(inner => outer != inner && inner.contains(outer)))

    //2 - Calculate suffix map for each dataset entry
    System.out.println("Building suffix map")
    val mappedStrings = nonContainedStrings.map(str => MappedString(str, buildSuffixMap(str))).toVector

    //3 - Run genetic algorithm on the string array
    Solver.Solve(mappedStrings)
  }
}
