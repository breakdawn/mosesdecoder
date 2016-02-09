/*
 * InputPaths.cpp
 *
 *  Created on: 23 Oct 2015
 *      Author: hieu
 */
#include <iostream>
#include "InputPaths.h"
#include "Phrase.h"
#include "System.h"
#include "legacy/Range.h"
#include "Search/Manager.h"

using namespace std;

namespace Moses2
{

InputPaths::~InputPaths()
{
}

void InputPaths::Init(const PhraseImpl &input, const Manager &mgr)
{
  MemPool &pool = mgr.GetPool();
  size_t numPt = mgr.system.mappings.size();
  size_t size = input.GetSize();
  size_t maxLength = min(size, mgr.system.maxPhraseLength);

  m_matrix = new (pool.Allocate< SquareMatrix<InputPath*> >()) SquareMatrix<InputPath*>(pool, size, maxLength);
  m_matrix->Init(NULL);

  // create blank path for initial hypo
  Range range(NOT_FOUND, NOT_FOUND);
  SubPhrase subPhrase = input.GetSubPhrase(NOT_FOUND, NOT_FOUND);
  m_blank = new (pool.Allocate<InputPath>()) InputPath(pool, subPhrase, range, numPt, NULL);

  // create normal paths of subphrases through the sentence
  for (size_t startPos = 0; startPos < size; ++startPos) {
	const InputPath *prefixPath = NULL;

    for (size_t phaseSize = 1; phaseSize <= maxLength; ++phaseSize) {
	  size_t endPos = startPos + phaseSize - 1;

	  if (endPos >= size) {
		  break;
	  }

	  SubPhrase subPhrase = input.GetSubPhrase(startPos, endPos);
	  Range range(startPos, endPos);

	  InputPath *path = new (pool.Allocate<InputPath>()) InputPath(pool, subPhrase, range, numPt, prefixPath);
	  m_inputPaths.push_back(path);

	  prefixPath = m_inputPaths.back();

	  m_matrix->SetValue(startPos, phaseSize - 1, path);
	}
  }

}

void InputPaths::DeleteUnusedPaths()
{
	size_t ind = 0;
	while (ind < m_inputPaths.size()) {
		const InputPath *path = m_inputPaths[ind];
		if (!path->IsUsed()) {
			m_inputPaths.erase(m_inputPaths.begin() + ind);
		}
		else {
			++ind;
		}
	}
}

}
