/*********************                                                        */
/*! \file theory_fp_type_rules.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Martin Brain, Tim King
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2017 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief [[ Add one-line brief description here ]]
 **
 ** [[ Add lengthier description here ]]
 ** \todo document this file
 **/

#include "cvc4_private.h"

#ifndef __CVC4__THEORY__FP__THEORY_FP_TYPE_RULES_H
#define __CVC4__THEORY__FP__THEORY_FP_TYPE_RULES_H

namespace CVC4 {
namespace theory {
namespace fp {

#define TRACE(FUNCTION)                                                \
  Trace("fp-type") << FUNCTION "::computeType(" << check << "): " << n \
                   << std::endl

class FloatingPointConstantTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointConstantTypeRule");

    const FloatingPoint& f = n.getConst<FloatingPoint>();

    if (check) {
      if (!(validExponentSize(f.t.exponent()))) {
        throw TypeCheckingExceptionPrivate(
            n, "constant with invalid exponent size");
      }
      if (!(validSignificandSize(f.t.significand()))) {
        throw TypeCheckingExceptionPrivate(
            n, "constant with invalid significand size");
      }
    }
    return nodeManager->mkFloatingPointType(f.t);
  }
};

class RoundingModeConstantTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("RoundingModeConstantTypeRule");

    // Nothing to check!
    return nodeManager->roundingModeType();
  }
};

class FloatingPointFPTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointFPTypeRule");

    TypeNode signType = n[0].getType(check);
    TypeNode exponentType = n[1].getType(check);
    TypeNode significandType = n[2].getType(check);

    if (!signType.isBitVector() || !exponentType.isBitVector() ||
        !significandType.isBitVector()) {
      throw TypeCheckingExceptionPrivate(n,
                                         "arguments to fp must be bit vectors");
    }

    unsigned signBits = signType.getBitVectorSize();
    unsigned exponentBits = exponentType.getBitVectorSize();
    unsigned significandBits = significandType.getBitVectorSize();

    if (check) {
      if (signBits != 1) {
        throw TypeCheckingExceptionPrivate(
            n, "sign bit vector in fp must be 1 bit long");
      } else if (!(validExponentSize(exponentBits))) {
        throw TypeCheckingExceptionPrivate(
            n, "exponent bit vector in fp is an invalid size");
      } else if (!(validSignificandSize(significandBits))) {
        throw TypeCheckingExceptionPrivate(
            n, "significand bit vector in fp is an invalid size");
      }
    }

    // The +1 is for the implicit hidden bit
    return nodeManager->mkFloatingPointType(exponentBits, significandBits + 1);
  }
};

class FloatingPointTestTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointTestTypeRule");

    if (check) {
      TypeNode firstOperand = n[0].getType(check);

      if (!firstOperand.isFloatingPoint()) {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point test applied to a non floating-point sort");
      }

      size_t children = n.getNumChildren();
      for (size_t i = 1; i < children; ++i) {
        if (!(n[i].getType(check) == firstOperand)) {
          throw TypeCheckingExceptionPrivate(
              n, "floating-point test applied to mixed sorts");
        }
      }
    }

    return nodeManager->booleanType();
  }
};

class FloatingPointOperationTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointOperationTypeRule");

    TypeNode firstOperand = n[0].getType(check);

    if (check) {
      if (!firstOperand.isFloatingPoint()) {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point operation applied to a non floating-point sort");
      }

      size_t children = n.getNumChildren();
      for (size_t i = 1; i < children; ++i) {
        if (!(n[i].getType(check) == firstOperand)) {
          throw TypeCheckingExceptionPrivate(
              n, "floating-point test applied to mixed sorts");
        }
      }
    }

    return firstOperand;
  }
};

class FloatingPointRoundingOperationTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointRoundingOperationTypeRule");

    if (check) {
      TypeNode roundingModeType = n[0].getType(check);

      if (!roundingModeType.isRoundingMode()) {
        throw TypeCheckingExceptionPrivate(
            n, "first argument must be a rounding mode");
      }
    }

    TypeNode firstOperand = n[1].getType(check);

    if (check) {
      if (!firstOperand.isFloatingPoint()) {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point operation applied to a non floating-point sort");
      }

      size_t children = n.getNumChildren();
      for (size_t i = 2; i < children; ++i) {
        if (!(n[i].getType(check) == firstOperand)) {
          throw TypeCheckingExceptionPrivate(
              n, "floating-point operation applied to mixed sorts");
        }
      }
    }

    return firstOperand;
  }
};

class FloatingPointPartialOperationTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointOperationTypeRule");
    AlwaysAssert(n.getNumChildren() > 0);

    TypeNode firstOperand = n[0].getType(check);

    if (check) {
      if (!firstOperand.isFloatingPoint()) {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point operation applied to a non floating-point sort");
      }

      const size_t children = n.getNumChildren();
      for (size_t i = 1; i < children - 1; ++i) {
        if (n[i].getType(check) != firstOperand) {
          throw TypeCheckingExceptionPrivate(
              n, "floating-point partial operation applied to mixed sorts");
        }
      }

      TypeNode UFValueType = n[children - 1].getType(check);

      if (!(UFValueType.isBitVector()) ||
	  !(UFValueType.getBitVectorSize() == 1)) {
	throw TypeCheckingExceptionPrivate(
	    n, "floating-point partial operation final argument must be a bit-vector of length 1");
      }
    }

    return firstOperand;
  }
};


class FloatingPointParametricOpTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointParametricOpTypeRule");

    return nodeManager->builtinOperatorType();
  }
};

class FloatingPointToFPIEEEBitVectorTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToFPIEEEBitVectorTypeRule");
    AlwaysAssert(n.getNumChildren() == 1);

    FloatingPointToFPIEEEBitVector info =
        n.getOperator().getConst<FloatingPointToFPIEEEBitVector>();

    if (check) {
      TypeNode operandType = n[0].getType(check);

      if (!(operandType.isBitVector())) {
        throw TypeCheckingExceptionPrivate(n,
                                           "conversion to floating-point from "
                                           "bit vector used with sort other "
                                           "than bit vector");
      } else if (!(operandType.getBitVectorSize() ==
                   info.t.exponent() + info.t.significand())) {
        throw TypeCheckingExceptionPrivate(
            n,
            "conversion to floating-point from bit vector used with bit vector "
            "length that does not match floating point parameters");
      }
    }

    return nodeManager->mkFloatingPointType(info.t);
  }
};

class FloatingPointToFPFloatingPointTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToFPFloatingPointTypeRule");
    AlwaysAssert(n.getNumChildren() == 2);

    FloatingPointToFPFloatingPoint info =
        n.getOperator().getConst<FloatingPointToFPFloatingPoint>();

    if (check) {
      TypeNode roundingModeType = n[0].getType(check);

      if (!roundingModeType.isRoundingMode()) {
        throw TypeCheckingExceptionPrivate(
            n, "first argument must be a rounding mode");
      }

      TypeNode operandType = n[1].getType(check);

      if (!(operandType.isFloatingPoint())) {
        throw TypeCheckingExceptionPrivate(n,
                                           "conversion to floating-point from "
                                           "floating-point used with sort "
                                           "other than floating-point");
      }
    }

    return nodeManager->mkFloatingPointType(info.t);
  }
};

class FloatingPointToFPRealTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToFPRealTypeRule");
    AlwaysAssert(n.getNumChildren() == 2);

    FloatingPointToFPReal info =
        n.getOperator().getConst<FloatingPointToFPReal>();

    if (check) {
      TypeNode roundingModeType = n[0].getType(check);

      if (!roundingModeType.isRoundingMode()) {
        throw TypeCheckingExceptionPrivate(
            n, "first argument must be a rounding mode");
      }

      TypeNode operandType = n[1].getType(check);

      if (!(operandType.isReal())) {
        throw TypeCheckingExceptionPrivate(n,
                                           "conversion to floating-point from "
                                           "real used with sort other than "
                                           "real");
      }
    }

    return nodeManager->mkFloatingPointType(info.t);
  }
};

class FloatingPointToFPSignedBitVectorTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToFPSignedBitVectorTypeRule");
    AlwaysAssert(n.getNumChildren() == 2);

    FloatingPointToFPSignedBitVector info =
        n.getOperator().getConst<FloatingPointToFPSignedBitVector>();

    if (check) {
      TypeNode roundingModeType = n[0].getType(check);

      if (!roundingModeType.isRoundingMode()) {
        throw TypeCheckingExceptionPrivate(
            n, "first argument must be a rounding mode");
      }

      TypeNode operandType = n[1].getType(check);

      if (!(operandType.isBitVector())) {
        throw TypeCheckingExceptionPrivate(n,
                                           "conversion to floating-point from "
                                           "signed bit vector used with sort "
                                           "other than bit vector");
      }
    }

    return nodeManager->mkFloatingPointType(info.t);
  }
};

class FloatingPointToFPUnsignedBitVectorTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToFPUnsignedBitVectorTypeRule");
    AlwaysAssert(n.getNumChildren() == 2);

    FloatingPointToFPUnsignedBitVector info =
        n.getOperator().getConst<FloatingPointToFPUnsignedBitVector>();

    if (check) {
      TypeNode roundingModeType = n[0].getType(check);

      if (!roundingModeType.isRoundingMode()) {
        throw TypeCheckingExceptionPrivate(
            n, "first argument must be a rounding mode");
      }

      TypeNode operandType = n[1].getType(check);

      if (!(operandType.isBitVector())) {
        throw TypeCheckingExceptionPrivate(n,
                                           "conversion to floating-point from "
                                           "unsigned bit vector used with sort "
                                           "other than bit vector");
      }
    }

    return nodeManager->mkFloatingPointType(info.t);
  }
};

class FloatingPointToFPGenericTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToFPGenericTypeRule");

    FloatingPointToFPGeneric info =
        n.getOperator().getConst<FloatingPointToFPGeneric>();

    if (check) {
      /* As this is a generic kind intended only for parsing,
       * the checking here is light.  For better checking, use
       * expandDefinitions first.
       */

      size_t children = n.getNumChildren();
      for (size_t i = 0; i < children; ++i) {
        n[i].getType(check);
      }
    }

    return nodeManager->mkFloatingPointType(info.t);
  }
};

class FloatingPointToUBVTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToUBVTypeRule");
    AlwaysAssert(n.getNumChildren() == 2);

    FloatingPointToUBV info = n.getOperator().getConst<FloatingPointToUBV>();

    if (check) {
      TypeNode roundingModeType = n[0].getType(check);

      if (!roundingModeType.isRoundingMode()) {
        throw TypeCheckingExceptionPrivate(
            n, "first argument must be a rounding mode");
      }

      TypeNode operandType = n[1].getType(check);

      if (!(operandType.isFloatingPoint())) {
        throw TypeCheckingExceptionPrivate(n,
                                           "conversion to unsigned bit vector "
                                           "used with a sort other than "
                                           "floating-point");
      }
    }

    return nodeManager->mkBitVectorType(info.bvs);
  }
};

class FloatingPointToSBVTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToSBVTypeRule");
    AlwaysAssert(n.getNumChildren() == 2);

    FloatingPointToSBV info = n.getOperator().getConst<FloatingPointToSBV>();

    if (check) {
      TypeNode roundingModeType = n[0].getType(check);

      if (!roundingModeType.isRoundingMode()) {
        throw TypeCheckingExceptionPrivate(
            n, "first argument must be a rounding mode");
      }

      TypeNode operandType = n[1].getType(check);

      if (!(operandType.isFloatingPoint())) {
        throw TypeCheckingExceptionPrivate(n,
                                           "conversion to signed bit vector "
                                           "used with a sort other than "
                                           "floating-point");
      }
    }

    return nodeManager->mkBitVectorType(info.bvs);
  }
};

class FloatingPointToUBVTotalTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToUBVTotalTypeRule");
    AlwaysAssert(n.getNumChildren() == 3);

    FloatingPointToUBVTotal info = n.getOperator().getConst<FloatingPointToUBVTotal>();

    if (check) {
      TypeNode roundingModeType = n[0].getType(check);

      if (!roundingModeType.isRoundingMode()) {
        throw TypeCheckingExceptionPrivate(
            n, "first argument must be a rounding mode");
      }

      TypeNode operandType = n[1].getType(check);

      if (!(operandType.isFloatingPoint())) {
        throw TypeCheckingExceptionPrivate(n,
                                           "conversion to unsigned bit vector total"
                                           "used with a sort other than "
                                           "floating-point");
      }

      TypeNode defaultValueType = n[2].getType(check);

      if (!(defaultValueType.isBitVector()) ||
	  !(defaultValueType.getBitVectorSize() == info)) {
	throw TypeCheckingExceptionPrivate(n,
					   "conversion to unsigned bit vector total"
					   "needs a bit vector of the same length"
					   "as last argument");
      }
    }

    return nodeManager->mkBitVectorType(info.bvs);
  }
};

class FloatingPointToSBVTotalTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToSBVTotalTypeRule");
    AlwaysAssert(n.getNumChildren() == 3);

    FloatingPointToSBVTotal info = n.getOperator().getConst<FloatingPointToSBVTotal>();

    if (check) {
      TypeNode roundingModeType = n[0].getType(check);

      if (!roundingModeType.isRoundingMode()) {
        throw TypeCheckingExceptionPrivate(
            n, "first argument must be a rounding mode");
      }

      TypeNode operandType = n[1].getType(check);

      if (!(operandType.isFloatingPoint())) {
        throw TypeCheckingExceptionPrivate(n,
                                           "conversion to signed bit vector "
                                           "used with a sort other than "
                                           "floating-point");
      }

      TypeNode defaultValueType = n[2].getType(check);

      if (!(defaultValueType.isBitVector()) ||
	  !(defaultValueType.getBitVectorSize() == info)) {
	throw TypeCheckingExceptionPrivate(n,
					   "conversion to signed bit vector total"
					   "needs a bit vector of the same length"
					   "as last argument");
      }
    }

    return nodeManager->mkBitVectorType(info.bvs);
  }
};

class FloatingPointToRealTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToRealTypeRule");
    AlwaysAssert(n.getNumChildren() == 1);

    if (check) {
      TypeNode operandType = n[0].getType(check);

      if (!operandType.isFloatingPoint()) {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point to real applied to a non floating-point sort");
      }
    }

    return nodeManager->realType();
  }
};

class FloatingPointToRealTotalTypeRule {
 public:
  inline static TypeNode computeType(NodeManager* nodeManager, TNode n,
                                     bool check) {
    TRACE("FloatingPointToRealTotalTypeRule");
    AlwaysAssert(n.getNumChildren() == 2);

    if (check) {
      TypeNode operandType = n[0].getType(check);

      if (!operandType.isFloatingPoint()) {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point to real total applied to a non floating-point sort");
      }

      TypeNode defaultValueType = n[1].getType(check);

      if (!defaultValueType.isReal()) {
        throw TypeCheckingExceptionPrivate(
            n, "floating-point to real total needs a real second argument");
      }

    }

    return nodeManager->realType();
  }
};


class CardinalityComputer {
public:
  inline static Cardinality computeCardinality(TypeNode type) {
    Assert(type.getKind() == kind::FLOATINGPOINT_TYPE);

    FloatingPointSize fps = type.getConst<FloatingPointSize>();

    /*
     * 1                    NaN
     * 2*1                  Infinities
     * 2*1                  Zeros
     * 2*2^(s-1)            Subnormal
     * 2*((2^e)-2)*2^(s-1)  Normal
     *
     *  = 1 + 2*2 + 2*((2^e)-1)*2^(s-1)
     *  =       5 + ((2^e)-1)*2^s
     */

    Integer significandValues = Integer(2).pow(fps.significand());
    Integer exponentValues = Integer(2).pow(fps.exponent());
    exponentValues -= Integer(1);

    return Integer(5) + exponentValues * significandValues;
  }
};/* class CardinalityComputer */




}/* CVC4::theory::fp namespace */
}/* CVC4::theory namespace */
}/* CVC4 namespace */

#endif /* __CVC4__THEORY__FP__THEORY_FP_TYPE_RULES_H */
