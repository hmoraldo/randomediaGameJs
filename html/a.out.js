// Note: Some Emscripten settings will significantly limit the speed of the generated code.
// Note: Some Emscripten settings may limit the speed of the generated code.
try {
  this['Module'] = Module;
  Module.test;
} catch(e) {
  this['Module'] = Module = {};
}
// The environment setup code below is customized to use Module.
// *** Environment setup code ***
var ENVIRONMENT_IS_NODE = typeof process === 'object' && typeof require === 'function';
var ENVIRONMENT_IS_WEB = typeof window === 'object';
var ENVIRONMENT_IS_WORKER = typeof importScripts === 'function';
var ENVIRONMENT_IS_SHELL = !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_NODE && !ENVIRONMENT_IS_WORKER;
if (typeof module === "object") {
  module.exports = Module;
}
if (ENVIRONMENT_IS_NODE) {
  // Expose functionality in the same simple way that the shells work
  // Note that we pollute the global namespace here, otherwise we break in node
  Module['print'] = function(x) {
    process['stdout'].write(x + '\n');
  };
  Module['printErr'] = function(x) {
    process['stderr'].write(x + '\n');
  };
  var nodeFS = require('fs');
  var nodePath = require('path');
  Module['read'] = function(filename, binary) {
    filename = nodePath['normalize'](filename);
    var ret = nodeFS['readFileSync'](filename);
    // The path is absolute if the normalized version is the same as the resolved.
    if (!ret && filename != nodePath['resolve'](filename)) {
      filename = path.join(__dirname, '..', 'src', filename);
      ret = nodeFS['readFileSync'](filename);
    }
    if (ret && !binary) ret = ret.toString();
    return ret;
  };
  Module['readBinary'] = function(filename) { return Module['read'](filename, true) };
  Module['load'] = function(f) {
    globalEval(read(f));
  };
  if (!Module['arguments']) {
    Module['arguments'] = process['argv'].slice(2);
  }
}
if (ENVIRONMENT_IS_SHELL) {
  Module['print'] = print;
  if (typeof printErr != 'undefined') Module['printErr'] = printErr; // not present in v8 or older sm
  Module['read'] = read;
  Module['readBinary'] = function(f) {
    return read(f, 'binary');
  };
  if (!Module['arguments']) {
    if (typeof scriptArgs != 'undefined') {
      Module['arguments'] = scriptArgs;
    } else if (typeof arguments != 'undefined') {
      Module['arguments'] = arguments;
    }
  }
}
if (ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_WORKER) {
  if (!Module['print']) {
    Module['print'] = function(x) {
      console.log(x);
    };
  }
  if (!Module['printErr']) {
    Module['printErr'] = function(x) {
      console.log(x);
    };
  }
}
if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
  Module['read'] = function(url) {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', url, false);
    xhr.send(null);
    return xhr.responseText;
  };
  if (!Module['arguments']) {
    if (typeof arguments != 'undefined') {
      Module['arguments'] = arguments;
    }
  }
}
if (ENVIRONMENT_IS_WORKER) {
  // We can do very little here...
  var TRY_USE_DUMP = false;
  if (!Module['print']) {
    Module['print'] = (TRY_USE_DUMP && (typeof(dump) !== "undefined") ? (function(x) {
      dump(x);
    }) : (function(x) {
      // self.postMessage(x); // enable this if you want stdout to be sent as messages
    }));
  }
  Module['load'] = importScripts;
}
if (!ENVIRONMENT_IS_WORKER && !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_NODE && !ENVIRONMENT_IS_SHELL) {
  // Unreachable because SHELL is dependant on the others
  throw 'Unknown runtime environment. Where are we?';
}
function globalEval(x) {
  eval.call(null, x);
}
if (!Module['load'] == 'undefined' && Module['read']) {
  Module['load'] = function(f) {
    globalEval(Module['read'](f));
  };
}
if (!Module['print']) {
  Module['print'] = function(){};
}
if (!Module['printErr']) {
  Module['printErr'] = Module['print'];
}
if (!Module['arguments']) {
  Module['arguments'] = [];
}
// *** Environment setup code ***
// Closure helpers
Module.print = Module['print'];
Module.printErr = Module['printErr'];
// Callbacks
if (!Module['preRun']) Module['preRun'] = [];
if (!Module['postRun']) Module['postRun'] = [];
// === Auto-generated preamble library stuff ===
//========================================
// Runtime code shared with compiler
//========================================
var Runtime = {
  stackSave: function () {
    return STACKTOP;
  },
  stackRestore: function (stackTop) {
    STACKTOP = stackTop;
  },
  forceAlign: function (target, quantum) {
    quantum = quantum || 4;
    if (quantum == 1) return target;
    if (isNumber(target) && isNumber(quantum)) {
      return Math.ceil(target/quantum)*quantum;
    } else if (isNumber(quantum) && isPowerOfTwo(quantum)) {
      var logg = log2(quantum);
      return '((((' +target + ')+' + (quantum-1) + ')>>' + logg + ')<<' + logg + ')';
    }
    return 'Math.ceil((' + target + ')/' + quantum + ')*' + quantum;
  },
  isNumberType: function (type) {
    return type in Runtime.INT_TYPES || type in Runtime.FLOAT_TYPES;
  },
  isPointerType: function isPointerType(type) {
  return type[type.length-1] == '*';
},
  isStructType: function isStructType(type) {
  if (isPointerType(type)) return false;
  if (isArrayType(type)) return true;
  if (/<?{ ?[^}]* ?}>?/.test(type)) return true; // { i32, i8 } etc. - anonymous struct types
  // See comment in isStructPointerType()
  return type[0] == '%';
},
  INT_TYPES: {"i1":0,"i8":0,"i16":0,"i32":0,"i64":0},
  FLOAT_TYPES: {"float":0,"double":0},
  or64: function (x, y) {
    var l = (x | 0) | (y | 0);
    var h = (Math.round(x / 4294967296) | Math.round(y / 4294967296)) * 4294967296;
    return l + h;
  },
  and64: function (x, y) {
    var l = (x | 0) & (y | 0);
    var h = (Math.round(x / 4294967296) & Math.round(y / 4294967296)) * 4294967296;
    return l + h;
  },
  xor64: function (x, y) {
    var l = (x | 0) ^ (y | 0);
    var h = (Math.round(x / 4294967296) ^ Math.round(y / 4294967296)) * 4294967296;
    return l + h;
  },
  getNativeTypeSize: function (type, quantumSize) {
    if (Runtime.QUANTUM_SIZE == 1) return 1;
    var size = {
      '%i1': 1,
      '%i8': 1,
      '%i16': 2,
      '%i32': 4,
      '%i64': 8,
      "%float": 4,
      "%double": 8
    }['%'+type]; // add '%' since float and double confuse Closure compiler as keys, and also spidermonkey as a compiler will remove 's from '_i8' etc
    if (!size) {
      if (type.charAt(type.length-1) == '*') {
        size = Runtime.QUANTUM_SIZE; // A pointer
      } else if (type[0] == 'i') {
        var bits = parseInt(type.substr(1));
        assert(bits % 8 == 0);
        size = bits/8;
      }
    }
    return size;
  },
  getNativeFieldSize: function (type) {
    return Math.max(Runtime.getNativeTypeSize(type), Runtime.QUANTUM_SIZE);
  },
  dedup: function dedup(items, ident) {
  var seen = {};
  if (ident) {
    return items.filter(function(item) {
      if (seen[item[ident]]) return false;
      seen[item[ident]] = true;
      return true;
    });
  } else {
    return items.filter(function(item) {
      if (seen[item]) return false;
      seen[item] = true;
      return true;
    });
  }
},
  set: function set() {
  var args = typeof arguments[0] === 'object' ? arguments[0] : arguments;
  var ret = {};
  for (var i = 0; i < args.length; i++) {
    ret[args[i]] = 0;
  }
  return ret;
},
  STACK_ALIGN: 8,
  getAlignSize: function (type, size, vararg) {
    // we align i64s and doubles on 64-bit boundaries, unlike x86
    if (type == 'i64' || type == 'double' || vararg) return 8;
    if (!type) return Math.min(size, 8); // align structures internally to 64 bits
    return Math.min(size || (type ? Runtime.getNativeFieldSize(type) : 0), Runtime.QUANTUM_SIZE);
  },
  calculateStructAlignment: function calculateStructAlignment(type) {
    type.flatSize = 0;
    type.alignSize = 0;
    var diffs = [];
    var prev = -1;
    type.flatIndexes = type.fields.map(function(field) {
      var size, alignSize;
      if (Runtime.isNumberType(field) || Runtime.isPointerType(field)) {
        size = Runtime.getNativeTypeSize(field); // pack char; char; in structs, also char[X]s.
        alignSize = Runtime.getAlignSize(field, size);
      } else if (Runtime.isStructType(field)) {
        size = Types.types[field].flatSize;
        alignSize = Runtime.getAlignSize(null, Types.types[field].alignSize);
      } else if (field[0] == 'b') {
        // bN, large number field, like a [N x i8]
        size = field.substr(1)|0;
        alignSize = 1;
      } else {
        throw 'Unclear type in struct: ' + field + ', in ' + type.name_ + ' :: ' + dump(Types.types[type.name_]);
      }
      if (type.packed) alignSize = 1;
      type.alignSize = Math.max(type.alignSize, alignSize);
      var curr = Runtime.alignMemory(type.flatSize, alignSize); // if necessary, place this on aligned memory
      type.flatSize = curr + size;
      if (prev >= 0) {
        diffs.push(curr-prev);
      }
      prev = curr;
      return curr;
    });
    type.flatSize = Runtime.alignMemory(type.flatSize, type.alignSize);
    if (diffs.length == 0) {
      type.flatFactor = type.flatSize;
    } else if (Runtime.dedup(diffs).length == 1) {
      type.flatFactor = diffs[0];
    }
    type.needsFlattening = (type.flatFactor != 1);
    return type.flatIndexes;
  },
  generateStructInfo: function (struct, typeName, offset) {
    var type, alignment;
    if (typeName) {
      offset = offset || 0;
      type = (typeof Types === 'undefined' ? Runtime.typeInfo : Types.types)[typeName];
      if (!type) return null;
      if (type.fields.length != struct.length) {
        printErr('Number of named fields must match the type for ' + typeName + ': possibly duplicate struct names. Cannot return structInfo');
        return null;
      }
      alignment = type.flatIndexes;
    } else {
      var type = { fields: struct.map(function(item) { return item[0] }) };
      alignment = Runtime.calculateStructAlignment(type);
    }
    var ret = {
      __size__: type.flatSize
    };
    if (typeName) {
      struct.forEach(function(item, i) {
        if (typeof item === 'string') {
          ret[item] = alignment[i] + offset;
        } else {
          // embedded struct
          var key;
          for (var k in item) key = k;
          ret[key] = Runtime.generateStructInfo(item[key], type.fields[i], alignment[i]);
        }
      });
    } else {
      struct.forEach(function(item, i) {
        ret[item[1]] = alignment[i];
      });
    }
    return ret;
  },
  dynCall: function (sig, ptr, args) {
    if (args && args.length) {
      assert(args.length == sig.length-1);
      return FUNCTION_TABLE[ptr].apply(null, args);
    } else {
      assert(sig.length == 1);
      return FUNCTION_TABLE[ptr]();
    }
  },
  addFunction: function (func) {
    var table = FUNCTION_TABLE;
    var ret = table.length;
    table.push(func);
    table.push(0);
    return ret;
  },
  removeFunction: function (index) {
    var table = FUNCTION_TABLE;
    table[index] = null;
  },
  warnOnce: function (text) {
    if (!Runtime.warnOnce.shown) Runtime.warnOnce.shown = {};
    if (!Runtime.warnOnce.shown[text]) {
      Runtime.warnOnce.shown[text] = 1;
      Module.printErr(text);
    }
  },
  funcWrappers: {},
  getFuncWrapper: function (func, sig) {
    assert(sig);
    if (!Runtime.funcWrappers[func]) {
      Runtime.funcWrappers[func] = function() {
        return Runtime.dynCall(sig, func, arguments);
      };
    }
    return Runtime.funcWrappers[func];
  },
  UTF8Processor: function () {
    var buffer = [];
    var needed = 0;
    this.processCChar = function (code) {
      code = code & 0xff;
      if (needed) {
        buffer.push(code);
        needed--;
      }
      if (buffer.length == 0) {
        if (code < 128) return String.fromCharCode(code);
        buffer.push(code);
        if (code > 191 && code < 224) {
          needed = 1;
        } else {
          needed = 2;
        }
        return '';
      }
      if (needed > 0) return '';
      var c1 = buffer[0];
      var c2 = buffer[1];
      var c3 = buffer[2];
      var ret;
      if (c1 > 191 && c1 < 224) {
        ret = String.fromCharCode(((c1 & 31) << 6) | (c2 & 63));
      } else {
        ret = String.fromCharCode(((c1 & 15) << 12) | ((c2 & 63) << 6) | (c3 & 63));
      }
      buffer.length = 0;
      return ret;
    }
    this.processJSString = function(string) {
      string = unescape(encodeURIComponent(string));
      var ret = [];
      for (var i = 0; i < string.length; i++) {
        ret.push(string.charCodeAt(i));
      }
      return ret;
    }
  },
  stackAlloc: function (size) { var ret = STACKTOP;STACKTOP = (STACKTOP + size)|0;STACKTOP = ((((STACKTOP)+7)>>3)<<3);assert((STACKTOP|0) < (STACK_MAX|0)); return ret; },
  staticAlloc: function (size) { var ret = STATICTOP;STATICTOP = (STATICTOP + (assert(!staticSealed),size))|0;STATICTOP = ((((STATICTOP)+7)>>3)<<3); return ret; },
  dynamicAlloc: function (size) { var ret = DYNAMICTOP;DYNAMICTOP = (DYNAMICTOP + (assert(DYNAMICTOP > 0),size))|0;DYNAMICTOP = ((((DYNAMICTOP)+7)>>3)<<3); if (DYNAMICTOP >= TOTAL_MEMORY) enlargeMemory();; return ret; },
  alignMemory: function (size,quantum) { var ret = size = Math.ceil((size)/(quantum ? quantum : 8))*(quantum ? quantum : 8); return ret; },
  makeBigInt: function (low,high,unsigned) { var ret = (unsigned ? (((low)>>>(0))+(((high)>>>(0))*4294967296)) : (((low)>>>(0))+(((high)|(0))*4294967296))); return ret; },
  GLOBAL_BASE: 8,
  QUANTUM_SIZE: 4,
  __dummy__: 0
}
//========================================
// Runtime essentials
//========================================
var __THREW__ = 0; // Used in checking for thrown exceptions.
var setjmpId = 1; // Used in setjmp/longjmp
var setjmpLabels = {};
var ABORT = false; // whether we are quitting the application. no code should run after this. set in exit() and abort()
var undef = 0;
// tempInt is used for 32-bit signed values or smaller. tempBigInt is used
// for 32-bit unsigned values or more than 32 bits. TODO: audit all uses of tempInt
var tempValue, tempInt, tempBigInt, tempInt2, tempBigInt2, tempPair, tempBigIntI, tempBigIntR, tempBigIntS, tempBigIntP, tempBigIntD;
var tempI64, tempI64b;
var tempRet0, tempRet1, tempRet2, tempRet3, tempRet4, tempRet5, tempRet6, tempRet7, tempRet8, tempRet9;
function abort(text) {
  Module.print(text + ':\n' + (new Error).stack);
  ABORT = true;
  throw "Assertion: " + text;
}
function assert(condition, text) {
  if (!condition) {
    abort('Assertion failed: ' + text);
  }
}
var globalScope = this;
// C calling interface. A convenient way to call C functions (in C files, or
// defined with extern "C").
//
// Note: LLVM optimizations can inline and remove functions, after which you will not be
//       able to call them. Closure can also do so. To avoid that, add your function to
//       the exports using something like
//
//         -s EXPORTED_FUNCTIONS='["_main", "_myfunc"]'
//
// @param ident      The name of the C function (note that C++ functions will be name-mangled - use extern "C")
// @param returnType The return type of the function, one of the JS types 'number', 'string' or 'array' (use 'number' for any C pointer, and
//                   'array' for JavaScript arrays and typed arrays).
// @param argTypes   An array of the types of arguments for the function (if there are no arguments, this can be ommitted). Types are as in returnType,
//                   except that 'array' is not possible (there is no way for us to know the length of the array)
// @param args       An array of the arguments to the function, as native JS values (as in returnType)
//                   Note that string arguments will be stored on the stack (the JS string will become a C string on the stack).
// @return           The return value, as a native JS value (as in returnType)
function ccall(ident, returnType, argTypes, args) {
  return ccallFunc(getCFunc(ident), returnType, argTypes, args);
}
Module["ccall"] = ccall;
// Returns the C function with a specified identifier (for C++, you need to do manual name mangling)
function getCFunc(ident) {
  try {
    var func = globalScope['Module']['_' + ident]; // closure exported function
    if (!func) func = eval('_' + ident); // explicit lookup
  } catch(e) {
  }
  assert(func, 'Cannot call unknown function ' + ident + ' (perhaps LLVM optimizations or closure removed it?)');
  return func;
}
// Internal function that does a C call using a function, not an identifier
function ccallFunc(func, returnType, argTypes, args) {
  var stack = 0;
  function toC(value, type) {
    if (type == 'string') {
      if (value === null || value === undefined || value === 0) return 0; // null string
      if (!stack) stack = Runtime.stackSave();
      var ret = Runtime.stackAlloc(value.length+1);
      writeStringToMemory(value, ret);
      return ret;
    } else if (type == 'array') {
      if (!stack) stack = Runtime.stackSave();
      var ret = Runtime.stackAlloc(value.length);
      writeArrayToMemory(value, ret);
      return ret;
    }
    return value;
  }
  function fromC(value, type) {
    if (type == 'string') {
      return Pointer_stringify(value);
    }
    assert(type != 'array');
    return value;
  }
  var i = 0;
  var cArgs = args ? args.map(function(arg) {
    return toC(arg, argTypes[i++]);
  }) : [];
  var ret = fromC(func.apply(null, cArgs), returnType);
  if (stack) Runtime.stackRestore(stack);
  return ret;
}
// Returns a native JS wrapper for a C function. This is similar to ccall, but
// returns a function you can call repeatedly in a normal way. For example:
//
//   var my_function = cwrap('my_c_function', 'number', ['number', 'number']);
//   alert(my_function(5, 22));
//   alert(my_function(99, 12));
//
function cwrap(ident, returnType, argTypes) {
  var func = getCFunc(ident);
  return function() {
    return ccallFunc(func, returnType, argTypes, Array.prototype.slice.call(arguments));
  }
}
Module["cwrap"] = cwrap;
// Sets a value in memory in a dynamic way at run-time. Uses the
// type data. This is the same as makeSetValue, except that
// makeSetValue is done at compile-time and generates the needed
// code then, whereas this function picks the right code at
// run-time.
// Note that setValue and getValue only do *aligned* writes and reads!
// Note that ccall uses JS types as for defining types, while setValue and
// getValue need LLVM types ('i8', 'i32') - this is a lower-level operation
function setValue(ptr, value, type, noSafe) {
  type = type || 'i8';
  if (type.charAt(type.length-1) === '*') type = 'i32'; // pointers are 32-bit
    switch(type) {
      case 'i1': HEAP8[(ptr)]=value; break;
      case 'i8': HEAP8[(ptr)]=value; break;
      case 'i16': HEAP16[((ptr)>>1)]=value; break;
      case 'i32': HEAP32[((ptr)>>2)]=value; break;
      case 'i64': (tempI64 = [value>>>0,Math.min(Math.floor((value)/4294967296), 4294967295)>>>0],HEAP32[((ptr)>>2)]=tempI64[0],HEAP32[(((ptr)+(4))>>2)]=tempI64[1]); break;
      case 'float': HEAPF32[((ptr)>>2)]=value; break;
      case 'double': HEAPF64[((ptr)>>3)]=value; break;
      default: abort('invalid type for setValue: ' + type);
    }
}
Module['setValue'] = setValue;
// Parallel to setValue.
function getValue(ptr, type, noSafe) {
  type = type || 'i8';
  if (type.charAt(type.length-1) === '*') type = 'i32'; // pointers are 32-bit
    switch(type) {
      case 'i1': return HEAP8[(ptr)];
      case 'i8': return HEAP8[(ptr)];
      case 'i16': return HEAP16[((ptr)>>1)];
      case 'i32': return HEAP32[((ptr)>>2)];
      case 'i64': return HEAP32[((ptr)>>2)];
      case 'float': return HEAPF32[((ptr)>>2)];
      case 'double': return HEAPF64[((ptr)>>3)];
      default: abort('invalid type for setValue: ' + type);
    }
  return null;
}
Module['getValue'] = getValue;
var ALLOC_NORMAL = 0; // Tries to use _malloc()
var ALLOC_STACK = 1; // Lives for the duration of the current function call
var ALLOC_STATIC = 2; // Cannot be freed
var ALLOC_DYNAMIC = 3; // Cannot be freed except through sbrk
var ALLOC_NONE = 4; // Do not allocate
Module['ALLOC_NORMAL'] = ALLOC_NORMAL;
Module['ALLOC_STACK'] = ALLOC_STACK;
Module['ALLOC_STATIC'] = ALLOC_STATIC;
Module['ALLOC_DYNAMIC'] = ALLOC_DYNAMIC;
Module['ALLOC_NONE'] = ALLOC_NONE;
// allocate(): This is for internal use. You can use it yourself as well, but the interface
//             is a little tricky (see docs right below). The reason is that it is optimized
//             for multiple syntaxes to save space in generated code. So you should
//             normally not use allocate(), and instead allocate memory using _malloc(),
//             initialize it with setValue(), and so forth.
// @slab: An array of data, or a number. If a number, then the size of the block to allocate,
//        in *bytes* (note that this is sometimes confusing: the next parameter does not
//        affect this!)
// @types: Either an array of types, one for each byte (or 0 if no type at that position),
//         or a single type which is used for the entire block. This only matters if there
//         is initial data - if @slab is a number, then this does not matter at all and is
//         ignored.
// @allocator: How to allocate memory, see ALLOC_*
function allocate(slab, types, allocator, ptr) {
  var zeroinit, size;
  if (typeof slab === 'number') {
    zeroinit = true;
    size = slab;
  } else {
    zeroinit = false;
    size = slab.length;
  }
  var singleType = typeof types === 'string' ? types : null;
  var ret;
  if (allocator == ALLOC_NONE) {
    ret = ptr;
  } else {
    ret = [_malloc, Runtime.stackAlloc, Runtime.staticAlloc, Runtime.dynamicAlloc][allocator === undefined ? ALLOC_STATIC : allocator](Math.max(size, singleType ? 1 : types.length));
  }
  if (zeroinit) {
    var ptr = ret, stop;
    assert((ret & 3) == 0);
    stop = ret + (size & ~3);
    for (; ptr < stop; ptr += 4) {
      HEAP32[((ptr)>>2)]=0;
    }
    stop = ret + size;
    while (ptr < stop) {
      HEAP8[((ptr++)|0)]=0;
    }
    return ret;
  }
  if (singleType === 'i8') {
    if (slab.subarray || slab.slice) {
      HEAPU8.set(slab, ret);
    } else {
      HEAPU8.set(new Uint8Array(slab), ret);
    }
    return ret;
  }
  var i = 0, type, typeSize, previousType;
  while (i < size) {
    var curr = slab[i];
    if (typeof curr === 'function') {
      curr = Runtime.getFunctionIndex(curr);
    }
    type = singleType || types[i];
    if (type === 0) {
      i++;
      continue;
    }
    assert(type, 'Must know what type to store in allocate!');
    if (type == 'i64') type = 'i32'; // special case: we have one i32 here, and one i32 later
    setValue(ret+i, curr, type);
    // no need to look up size unless type changes, so cache it
    if (previousType !== type) {
      typeSize = Runtime.getNativeTypeSize(type);
      previousType = type;
    }
    i += typeSize;
  }
  return ret;
}
Module['allocate'] = allocate;
function Pointer_stringify(ptr, /* optional */ length) {
  // Find the length, and check for UTF while doing so
  var hasUtf = false;
  var t;
  var i = 0;
  while (1) {
    t = HEAPU8[(((ptr)+(i))|0)];
    if (t >= 128) hasUtf = true;
    else if (t == 0 && !length) break;
    i++;
    if (length && i == length) break;
  }
  if (!length) length = i;
  var ret = '';
  if (!hasUtf) {
    var MAX_CHUNK = 1024; // split up into chunks, because .apply on a huge string can overflow the stack
    var curr;
    while (length > 0) {
      curr = String.fromCharCode.apply(String, HEAPU8.subarray(ptr, ptr + Math.min(length, MAX_CHUNK)));
      ret = ret ? ret + curr : curr;
      ptr += MAX_CHUNK;
      length -= MAX_CHUNK;
    }
    return ret;
  }
  var utf8 = new Runtime.UTF8Processor();
  for (i = 0; i < length; i++) {
    assert(ptr + i < TOTAL_MEMORY);
    t = HEAPU8[(((ptr)+(i))|0)];
    ret += utf8.processCChar(t);
  }
  return ret;
}
Module['Pointer_stringify'] = Pointer_stringify;
// Memory management
var PAGE_SIZE = 4096;
function alignMemoryPage(x) {
  return ((x+4095)>>12)<<12;
}
var HEAP;
var HEAP8, HEAPU8, HEAP16, HEAPU16, HEAP32, HEAPU32, HEAPF32, HEAPF64;
var STATIC_BASE = 0, STATICTOP = 0, staticSealed = false; // static area
var STACK_BASE = 0, STACKTOP = 0, STACK_MAX = 0; // stack area
var DYNAMIC_BASE = 0, DYNAMICTOP = 0; // dynamic area handled by sbrk
function enlargeMemory() {
  abort('Cannot enlarge memory arrays. Either (1) compile with -s TOTAL_MEMORY=X with X higher than the current value, (2) compile with ALLOW_MEMORY_GROWTH which adjusts the size at runtime but prevents some optimizations, or (3) set Module.TOTAL_MEMORY before the program runs.');
}
var TOTAL_STACK = Module['TOTAL_STACK'] || 5242880;
var TOTAL_MEMORY = Module['TOTAL_MEMORY'] || 16777216;
var FAST_MEMORY = Module['FAST_MEMORY'] || 2097152;
// Initialize the runtime's memory
// check for full engine support (use string 'subarray' to avoid closure compiler confusion)
assert(!!Int32Array && !!Float64Array && !!(new Int32Array(1)['subarray']) && !!(new Int32Array(1)['set']),
       'Cannot fallback to non-typed array case: Code is too specialized');
var buffer = new ArrayBuffer(TOTAL_MEMORY);
HEAP8 = new Int8Array(buffer);
HEAP16 = new Int16Array(buffer);
HEAP32 = new Int32Array(buffer);
HEAPU8 = new Uint8Array(buffer);
HEAPU16 = new Uint16Array(buffer);
HEAPU32 = new Uint32Array(buffer);
HEAPF32 = new Float32Array(buffer);
HEAPF64 = new Float64Array(buffer);
// Endianness check (note: assumes compiler arch was little-endian)
HEAP32[0] = 255;
assert(HEAPU8[0] === 255 && HEAPU8[3] === 0, 'Typed arrays 2 must be run on a little-endian system');
Module['HEAP'] = HEAP;
Module['HEAP8'] = HEAP8;
Module['HEAP16'] = HEAP16;
Module['HEAP32'] = HEAP32;
Module['HEAPU8'] = HEAPU8;
Module['HEAPU16'] = HEAPU16;
Module['HEAPU32'] = HEAPU32;
Module['HEAPF32'] = HEAPF32;
Module['HEAPF64'] = HEAPF64;
function callRuntimeCallbacks(callbacks) {
  while(callbacks.length > 0) {
    var callback = callbacks.shift();
    if (typeof callback == 'function') {
      callback();
      continue;
    }
    var func = callback.func;
    if (typeof func === 'number') {
      if (callback.arg === undefined) {
        Runtime.dynCall('v', func);
      } else {
        Runtime.dynCall('vi', func, [callback.arg]);
      }
    } else {
      func(callback.arg === undefined ? null : callback.arg);
    }
  }
}
var __ATINIT__ = []; // functions called during startup
var __ATMAIN__ = []; // functions called when main() is to be run
var __ATEXIT__ = []; // functions called during shutdown
var runtimeInitialized = false;
function ensureInitRuntime() {
  if (runtimeInitialized) return;
  runtimeInitialized = true;
  callRuntimeCallbacks(__ATINIT__);
}
function preMain() {
  callRuntimeCallbacks(__ATMAIN__);
}
function exitRuntime() {
  callRuntimeCallbacks(__ATEXIT__);
}
// Tools
// This processes a JS string into a C-line array of numbers, 0-terminated.
// For LLVM-originating strings, see parser.js:parseLLVMString function
function intArrayFromString(stringy, dontAddNull, length /* optional */) {
  var ret = (new Runtime.UTF8Processor()).processJSString(stringy);
  if (length) {
    ret.length = length;
  }
  if (!dontAddNull) {
    ret.push(0);
  }
  return ret;
}
Module['intArrayFromString'] = intArrayFromString;
function intArrayToString(array) {
  var ret = [];
  for (var i = 0; i < array.length; i++) {
    var chr = array[i];
    if (chr > 0xFF) {
        assert(false, 'Character code ' + chr + ' (' + String.fromCharCode(chr) + ')  at offset ' + i + ' not in 0x00-0xFF.');
      chr &= 0xFF;
    }
    ret.push(String.fromCharCode(chr));
  }
  return ret.join('');
}
Module['intArrayToString'] = intArrayToString;
// Write a Javascript array to somewhere in the heap
function writeStringToMemory(string, buffer, dontAddNull) {
  var array = intArrayFromString(string, dontAddNull);
  var i = 0;
  while (i < array.length) {
    var chr = array[i];
    HEAP8[(((buffer)+(i))|0)]=chr
    i = i + 1;
  }
}
Module['writeStringToMemory'] = writeStringToMemory;
function writeArrayToMemory(array, buffer) {
  for (var i = 0; i < array.length; i++) {
    HEAP8[(((buffer)+(i))|0)]=array[i];
  }
}
Module['writeArrayToMemory'] = writeArrayToMemory;
function unSign(value, bits, ignore, sig) {
  if (value >= 0) {
    return value;
  }
  return bits <= 32 ? 2*Math.abs(1 << (bits-1)) + value // Need some trickery, since if bits == 32, we are right at the limit of the bits JS uses in bitshifts
                    : Math.pow(2, bits)         + value;
}
function reSign(value, bits, ignore, sig) {
  if (value <= 0) {
    return value;
  }
  var half = bits <= 32 ? Math.abs(1 << (bits-1)) // abs is needed if bits == 32
                        : Math.pow(2, bits-1);
  if (value >= half && (bits <= 32 || value > half)) { // for huge values, we can hit the precision limit and always get true here. so don't do that
                                                       // but, in general there is no perfect solution here. With 64-bit ints, we get rounding and errors
                                                       // TODO: In i64 mode 1, resign the two parts separately and safely
    value = -2*half + value; // Cannot bitshift half, as it may be at the limit of the bits JS uses in bitshifts
  }
  return value;
}
if (!Math['imul']) Math['imul'] = function(a, b) {
  var ah  = a >>> 16;
  var al = a & 0xffff;
  var bh  = b >>> 16;
  var bl = b & 0xffff;
  return (al*bl + ((ah*bl + al*bh) << 16))|0;
};
// A counter of dependencies for calling run(). If we need to
// do asynchronous work before running, increment this and
// decrement it. Incrementing must happen in a place like
// PRE_RUN_ADDITIONS (used by emcc to add file preloading).
// Note that you can add dependencies in preRun, even though
// it happens right before run - run will be postponed until
// the dependencies are met.
var runDependencies = 0;
var runDependencyTracking = {};
var calledInit = false, calledRun = false;
var runDependencyWatcher = null;
function addRunDependency(id) {
  runDependencies++;
  if (Module['monitorRunDependencies']) {
    Module['monitorRunDependencies'](runDependencies);
  }
  if (id) {
    assert(!runDependencyTracking[id]);
    runDependencyTracking[id] = 1;
    if (runDependencyWatcher === null && typeof setInterval !== 'undefined') {
      // Check for missing dependencies every few seconds
      runDependencyWatcher = setInterval(function() {
        var shown = false;
        for (var dep in runDependencyTracking) {
          if (!shown) {
            shown = true;
            Module.printErr('still waiting on run dependencies:');
          }
          Module.printErr('dependency: ' + dep);
        }
        if (shown) {
          Module.printErr('(end of list)');
        }
      }, 10000);
    }
  } else {
    Module.printErr('warning: run dependency added without ID');
  }
}
Module['addRunDependency'] = addRunDependency;
function removeRunDependency(id) {
  runDependencies--;
  if (Module['monitorRunDependencies']) {
    Module['monitorRunDependencies'](runDependencies);
  }
  if (id) {
    assert(runDependencyTracking[id]);
    delete runDependencyTracking[id];
  } else {
    Module.printErr('warning: run dependency removed without ID');
  }
  if (runDependencies == 0) {
    if (runDependencyWatcher !== null) {
      clearInterval(runDependencyWatcher);
      runDependencyWatcher = null;
    } 
    // If run has never been called, and we should call run (INVOKE_RUN is true, and Module.noInitialRun is not false)
    if (!calledRun && shouldRunNow) run();
  }
}
Module['removeRunDependency'] = removeRunDependency;
Module["preloadedImages"] = {}; // maps url to image data
Module["preloadedAudios"] = {}; // maps url to audio data
function addPreRun(func) {
  if (!Module['preRun']) Module['preRun'] = [];
  else if (typeof Module['preRun'] == 'function') Module['preRun'] = [Module['preRun']];
  Module['preRun'].push(func);
}
var awaitingMemoryInitializer = false;
function loadMemoryInitializer(filename) {
  function applyData(data) {
    HEAPU8.set(data, STATIC_BASE);
    runPostSets();
  }
  // always do this asynchronously, to keep shell and web as similar as possible
  addPreRun(function() {
    if (ENVIRONMENT_IS_NODE || ENVIRONMENT_IS_SHELL) {
      applyData(Module['readBinary'](filename));
    } else {
      Browser.asyncLoad(filename, function(data) {
        applyData(data);
      }, function(data) {
        throw 'could not load memory initializer ' + filename;
      });
    }
  });
  awaitingMemoryInitializer = false;
}
// === Body ===
STATIC_BASE = 8;
STATICTOP = STATIC_BASE + 1792;
var _logger;
__ATINIT__ = __ATINIT__.concat([
  { func: function() { __GLOBAL__I_a() } },
  { func: function() { __GLOBAL__I_a48() } }
]);
var ___dso_handle;
var __ZTVN10__cxxabiv120__si_class_type_infoE;
var __ZTVN10__cxxabiv117__class_type_infoE;
var __ZN10__cxxabiv117__class_type_infoD1Ev;
var __ZN10__cxxabiv117__class_type_infoD2Ev;
var __ZN10__cxxabiv120__si_class_type_infoD1Ev;
var __ZN10__cxxabiv120__si_class_type_infoD2Ev;
__ZTVN10__cxxabiv120__si_class_type_infoE=allocate([0,0,0,0,24,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], "i8", ALLOC_STATIC);
__ZTVN10__cxxabiv117__class_type_infoE=allocate([0,0,0,0,40,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], "i8", ALLOC_STATIC);
/* memory initializer */ allocate([67,82,101,100,105,97,32,45,32,86,101,114,46,32,49,46,48,49,32,45,32,65,112,114,32,32,49,32,50,48,49,52,32,45,32,49,56,58,53,56,58,48,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,200,0,0,0,0,0,0,0,68,68,46,115,101,116,86,105,100,101,111,77,111,100,101,32,102,97,105,108,101,100,46,0,68,68,46,105,110,105,116,32,102,97,105,108,101,100,46,0,69,120,101,99,117,116,97,98,108,101,32,105,110,102,111,58,32,37,115,0,0,0,0,0,100,97,116,97,47,108,111,103,115,46,116,120,116,0,0,0,76,111,103,103,105,110,103,32,105,110,32,100,101,118,101,108,111,112,101,114,32,109,111,100,101,46,0,0,0,0,0,0,42,42,42,42,42,42,42,42,42,42,42,42,0,0,0,0,98,108,97,32,98,108,97,32,98,108,97,10,0,0,0,0,0,0,0,0,0,0,0,0,67,111,117,108,100,32,110,111,116,32,115,101,116,32,117,112,32,102,105,114,115,116,32,112,114,111,103,114,97,109,32,122,111,110,101,46,46,46,0,0,78,111,110,114,101,99,111,118,101,114,32,101,114,114,111,114,32,119,104,101,110,32,114,101,115,116,111,114,105,110,103,32,114,101,115,111,117,114,99,101,115,0,0,0,0,0,0,0,70,114,97,109,101,115,32,112,101,114,32,115,101,99,111,110,100,58,32,37,102,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,72,6,0,0,36,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,6,0,0,116,0,0,0,62,0,0,0,54,0,0,0,72,0,0,0,124,0,0,0,126,0,0,0,44,0,0,0,32,0,0,0,42,0,0,0,16,0,0,0,16,0,0,0,16,0,0,0,16,0,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,6,0,0,26,0,0,0,14,0,0,0,80,0,0,0,56,0,0,0,24,0,0,0,38,0,0,0,108,0,0,0,86,0,0,0,30,0,0,0,52,0,0,0,130,0,0,0,92,0,0,0,20,0,0,0,110,0,0,0,120,0,0,0,34,0,0,0,96,0,0,0,112,0,0,0,70,0,0,0,94,0,0,0,8,0,0,0,76,0,0,0,82,0,0,0,4,0,0,0,104,0,0,0,90,0,0,0,74,0,0,0,88,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,104,6,0,0,132,0,0,0,78,0,0,0,80,0,0,0,56,0,0,0,24,0,0,0,38,0,0,0,108,0,0,0,86,0,0,0,30,0,0,0,52,0,0,0,130,0,0,0,92,0,0,0,20,0,0,0,110,0,0,0,120,0,0,0,34,0,0,0,96,0,0,0,112,0,0,0,70,0,0,0,94,0,0,0,8,0,0,0,76,0,0,0,82,0,0,0,4,0,0,0,28,0,0,0,46,0,0,0,16,0,0,0,88,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,6,0,0,98,0,0,0,100,0,0,0,54,0,0,0,72,0,0,0,124,0,0,0,126,0,0,0,44,0,0,0,32,0,0,0,42,0,0,0,106,0,0,0,114,0,0,0,50,0,0,0,48,0,0,0,128,0,0,0,0,0,0,0,0,0,0,0,83,116,57,116,121,112,101,95,105,110,102,111,0,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,50,48,95,95,115,105,95,99,108,97,115,115,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,49,55,95,95,99,108,97,115,115,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,49,54,95,95,115,104,105,109,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,0,0,0,0,49,54,116,105,109,101,87,114,97,112,112,101,114,67,108,97,115,115,0,0,0,0,0,0,49,54,108,111,111,112,87,114,97,112,112,101,114,67,108,97,115,115,0,0,0,0,0,0,49,53,100,105,114,101,99,116,68,114,97,119,67,108,97,115,115,0,0,0,0,0,0,0,49,52,100,100,83,117,114,102,97,99,101,67,108,97,115,115,0,0,0,0,0,0,0,0,49,51,109,97,105,110,76,111,111,112,67,108,97,115,115,0,0,0,0,0,24,5,0,0,0,0,0,0,40,5,0,0,40,6,0,0,0,0,0,0,0,0,0,0,80,5,0,0,56,6,0,0,0,0,0,0,0,0,0,0,120,5,0,0,16,6,0,0,0,0,0,0,0,0,0,0,160,5,0,0,0,0,0,0,184,5,0,0,0,0,0,0,208,5,0,0,104,6,0,0,0,0,0,0,0,0,0,0,232,5,0,0,0,0,0,0,0,6,0,0,80,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], "i8", ALLOC_NONE, Runtime.GLOBAL_BASE)
function runPostSets() {
HEAP32[(((__ZTVN10__cxxabiv120__si_class_type_infoE)+(8))>>2)]=(102);
HEAP32[(((__ZTVN10__cxxabiv120__si_class_type_infoE)+(12))>>2)]=(58);
HEAP32[(((__ZTVN10__cxxabiv120__si_class_type_infoE)+(16))>>2)]=(84);
HEAP32[(((__ZTVN10__cxxabiv120__si_class_type_infoE)+(20))>>2)]=(118);
HEAP32[(((__ZTVN10__cxxabiv120__si_class_type_infoE)+(24))>>2)]=(64);
HEAP32[(((__ZTVN10__cxxabiv120__si_class_type_infoE)+(28))>>2)]=(66);
HEAP32[(((__ZTVN10__cxxabiv120__si_class_type_infoE)+(32))>>2)]=(122);
HEAP32[(((__ZTVN10__cxxabiv120__si_class_type_infoE)+(36))>>2)]=(134);
HEAP32[(((__ZTVN10__cxxabiv117__class_type_infoE)+(8))>>2)]=(18);
HEAP32[(((__ZTVN10__cxxabiv117__class_type_infoE)+(12))>>2)]=(22);
HEAP32[(((__ZTVN10__cxxabiv117__class_type_infoE)+(16))>>2)]=(84);
HEAP32[(((__ZTVN10__cxxabiv117__class_type_infoE)+(20))>>2)]=(118);
HEAP32[(((__ZTVN10__cxxabiv117__class_type_infoE)+(24))>>2)]=(64);
HEAP32[(((__ZTVN10__cxxabiv117__class_type_infoE)+(28))>>2)]=(10);
HEAP32[(((__ZTVN10__cxxabiv117__class_type_infoE)+(32))>>2)]=(68);
HEAP32[(((__ZTVN10__cxxabiv117__class_type_infoE)+(36))>>2)]=(40);
HEAP32[((1552)>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((1560)>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((1576)>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((1592)>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((1608)>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((1616)>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((1624)>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((1640)>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((1648)>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
__ZN10__cxxabiv117__class_type_infoD1Ev = 12;
__ZN10__cxxabiv117__class_type_infoD2Ev = (60);
__ZN10__cxxabiv120__si_class_type_infoD1Ev = 2;
__ZN10__cxxabiv120__si_class_type_infoD2Ev = (12);
}
if (!awaitingMemoryInitializer) runPostSets();
var tempDoublePtr = Runtime.alignMemory(allocate(12, "i8", ALLOC_STATIC), 8);
assert(tempDoublePtr % 8 == 0);
function copyTempFloat(ptr) { // functions, because inlining this code increases code size too much
  HEAP8[tempDoublePtr] = HEAP8[ptr];
  HEAP8[tempDoublePtr+1] = HEAP8[ptr+1];
  HEAP8[tempDoublePtr+2] = HEAP8[ptr+2];
  HEAP8[tempDoublePtr+3] = HEAP8[ptr+3];
}
function copyTempDouble(ptr) {
  HEAP8[tempDoublePtr] = HEAP8[ptr];
  HEAP8[tempDoublePtr+1] = HEAP8[ptr+1];
  HEAP8[tempDoublePtr+2] = HEAP8[ptr+2];
  HEAP8[tempDoublePtr+3] = HEAP8[ptr+3];
  HEAP8[tempDoublePtr+4] = HEAP8[ptr+4];
  HEAP8[tempDoublePtr+5] = HEAP8[ptr+5];
  HEAP8[tempDoublePtr+6] = HEAP8[ptr+6];
  HEAP8[tempDoublePtr+7] = HEAP8[ptr+7];
}
  var _llvm_dbg_declare=undefined;
  var _llvm_va_start=undefined;
  var ERRNO_CODES={EPERM:1,ENOENT:2,ESRCH:3,EINTR:4,EIO:5,ENXIO:6,E2BIG:7,ENOEXEC:8,EBADF:9,ECHILD:10,EAGAIN:11,EWOULDBLOCK:11,ENOMEM:12,EACCES:13,EFAULT:14,ENOTBLK:15,EBUSY:16,EEXIST:17,EXDEV:18,ENODEV:19,ENOTDIR:20,EISDIR:21,EINVAL:22,ENFILE:23,EMFILE:24,ENOTTY:25,ETXTBSY:26,EFBIG:27,ENOSPC:28,ESPIPE:29,EROFS:30,EMLINK:31,EPIPE:32,EDOM:33,ERANGE:34,ENOMSG:35,EIDRM:36,ECHRNG:37,EL2NSYNC:38,EL3HLT:39,EL3RST:40,ELNRNG:41,EUNATCH:42,ENOCSI:43,EL2HLT:44,EDEADLK:45,ENOLCK:46,EBADE:50,EBADR:51,EXFULL:52,ENOANO:53,EBADRQC:54,EBADSLT:55,EDEADLOCK:56,EBFONT:57,ENOSTR:60,ENODATA:61,ETIME:62,ENOSR:63,ENONET:64,ENOPKG:65,EREMOTE:66,ENOLINK:67,EADV:68,ESRMNT:69,ECOMM:70,EPROTO:71,EMULTIHOP:74,ELBIN:75,EDOTDOT:76,EBADMSG:77,EFTYPE:79,ENOTUNIQ:80,EBADFD:81,EREMCHG:82,ELIBACC:83,ELIBBAD:84,ELIBSCN:85,ELIBMAX:86,ELIBEXEC:87,ENOSYS:88,ENMFILE:89,ENOTEMPTY:90,ENAMETOOLONG:91,ELOOP:92,EOPNOTSUPP:95,EPFNOSUPPORT:96,ECONNRESET:104,ENOBUFS:105,EAFNOSUPPORT:106,EPROTOTYPE:107,ENOTSOCK:108,ENOPROTOOPT:109,ESHUTDOWN:110,ECONNREFUSED:111,EADDRINUSE:112,ECONNABORTED:113,ENETUNREACH:114,ENETDOWN:115,ETIMEDOUT:116,EHOSTDOWN:117,EHOSTUNREACH:118,EINPROGRESS:119,EALREADY:120,EDESTADDRREQ:121,EMSGSIZE:122,EPROTONOSUPPORT:123,ESOCKTNOSUPPORT:124,EADDRNOTAVAIL:125,ENETRESET:126,EISCONN:127,ENOTCONN:128,ETOOMANYREFS:129,EPROCLIM:130,EUSERS:131,EDQUOT:132,ESTALE:133,ENOTSUP:134,ENOMEDIUM:135,ENOSHARE:136,ECASECLASH:137,EILSEQ:138,EOVERFLOW:139,ECANCELED:140,ENOTRECOVERABLE:141,EOWNERDEAD:142,ESTRPIPE:143};
  var ___errno_state=0;function ___setErrNo(value) {
      // For convenient setting and returning of errno.
      HEAP32[((___errno_state)>>2)]=value
      return value;
    }
  var _stdin=allocate(1, "i32*", ALLOC_STATIC);
  var _stdout=allocate(1, "i32*", ALLOC_STATIC);
  var _stderr=allocate(1, "i32*", ALLOC_STATIC);
  var __impure_ptr=allocate(1, "i32*", ALLOC_STATIC);var FS={currentPath:"/",nextInode:2,streams:[null],checkStreams:function () {
        for (var i in FS.streams) if (FS.streams.hasOwnProperty(i)) assert(i >= 0 && i < FS.streams.length); // no keys not in dense span
        for (var i = 0; i < FS.streams.length; i++) assert(typeof FS.streams[i] == 'object'); // no non-null holes in dense span
      },ignorePermissions:true,createFileHandle:function (stream, fd) {
        if (typeof stream === 'undefined') {
          stream = null;
        }
        if (!fd) {
          if (stream && stream.socket) {
            for (var i = 1; i < 64; i++) {
              if (!FS.streams[i]) {
                fd = i;
                break;
              }
            }
            assert(fd, 'ran out of low fds for sockets');
          } else {
            fd = Math.max(FS.streams.length, 64);
            for (var i = FS.streams.length; i < fd; i++) {
              FS.streams[i] = null; // Keep dense
            }
          }
        }
        // Close WebSocket first if we are about to replace the fd (i.e. dup2)
        if (FS.streams[fd] && FS.streams[fd].socket && FS.streams[fd].socket.close) {
          FS.streams[fd].socket.close();
        }
        FS.streams[fd] = stream;
        return fd;
      },removeFileHandle:function (fd) {
        FS.streams[fd] = null;
      },joinPath:function (parts, forceRelative) {
        var ret = parts[0];
        for (var i = 1; i < parts.length; i++) {
          if (ret[ret.length-1] != '/') ret += '/';
          ret += parts[i];
        }
        if (forceRelative && ret[0] == '/') ret = ret.substr(1);
        return ret;
      },absolutePath:function (relative, base) {
        if (typeof relative !== 'string') return null;
        if (base === undefined) base = FS.currentPath;
        if (relative && relative[0] == '/') base = '';
        var full = base + '/' + relative;
        var parts = full.split('/').reverse();
        var absolute = [''];
        while (parts.length) {
          var part = parts.pop();
          if (part == '' || part == '.') {
            // Nothing.
          } else if (part == '..') {
            if (absolute.length > 1) absolute.pop();
          } else {
            absolute.push(part);
          }
        }
        return absolute.length == 1 ? '/' : absolute.join('/');
      },analyzePath:function (path, dontResolveLastLink, linksVisited) {
        var ret = {
          isRoot: false,
          exists: false,
          error: 0,
          name: null,
          path: null,
          object: null,
          parentExists: false,
          parentPath: null,
          parentObject: null
        };
        path = FS.absolutePath(path);
        if (path == '/') {
          ret.isRoot = true;
          ret.exists = ret.parentExists = true;
          ret.name = '/';
          ret.path = ret.parentPath = '/';
          ret.object = ret.parentObject = FS.root;
        } else if (path !== null) {
          linksVisited = linksVisited || 0;
          path = path.slice(1).split('/');
          var current = FS.root;
          var traversed = [''];
          while (path.length) {
            if (path.length == 1 && current.isFolder) {
              ret.parentExists = true;
              ret.parentPath = traversed.length == 1 ? '/' : traversed.join('/');
              ret.parentObject = current;
              ret.name = path[0];
            }
            var target = path.shift();
            if (!current.isFolder) {
              ret.error = ERRNO_CODES.ENOTDIR;
              break;
            } else if (!current.read) {
              ret.error = ERRNO_CODES.EACCES;
              break;
            } else if (!current.contents.hasOwnProperty(target)) {
              ret.error = ERRNO_CODES.ENOENT;
              break;
            }
            current = current.contents[target];
            if (current.link && !(dontResolveLastLink && path.length == 0)) {
              if (linksVisited > 40) { // Usual Linux SYMLOOP_MAX.
                ret.error = ERRNO_CODES.ELOOP;
                break;
              }
              var link = FS.absolutePath(current.link, traversed.join('/'));
              ret = FS.analyzePath([link].concat(path).join('/'),
                                   dontResolveLastLink, linksVisited + 1);
              return ret;
            }
            traversed.push(target);
            if (path.length == 0) {
              ret.exists = true;
              ret.path = traversed.join('/');
              ret.object = current;
            }
          }
        }
        return ret;
      },findObject:function (path, dontResolveLastLink) {
        FS.ensureRoot();
        var ret = FS.analyzePath(path, dontResolveLastLink);
        if (ret.exists) {
          return ret.object;
        } else {
          ___setErrNo(ret.error);
          return null;
        }
      },createObject:function (parent, name, properties, canRead, canWrite) {
        if (!parent) parent = '/';
        if (typeof parent === 'string') parent = FS.findObject(parent);
        if (!parent) {
          ___setErrNo(ERRNO_CODES.EACCES);
          throw new Error('Parent path must exist.');
        }
        if (!parent.isFolder) {
          ___setErrNo(ERRNO_CODES.ENOTDIR);
          throw new Error('Parent must be a folder.');
        }
        if (!parent.write && !FS.ignorePermissions) {
          ___setErrNo(ERRNO_CODES.EACCES);
          throw new Error('Parent folder must be writeable.');
        }
        if (!name || name == '.' || name == '..') {
          ___setErrNo(ERRNO_CODES.ENOENT);
          throw new Error('Name must not be empty.');
        }
        if (parent.contents.hasOwnProperty(name)) {
          ___setErrNo(ERRNO_CODES.EEXIST);
          throw new Error("Can't overwrite object.");
        }
        parent.contents[name] = {
          read: canRead === undefined ? true : canRead,
          write: canWrite === undefined ? false : canWrite,
          timestamp: Date.now(),
          inodeNumber: FS.nextInode++
        };
        for (var key in properties) {
          if (properties.hasOwnProperty(key)) {
            parent.contents[name][key] = properties[key];
          }
        }
        return parent.contents[name];
      },createFolder:function (parent, name, canRead, canWrite) {
        var properties = {isFolder: true, isDevice: false, contents: {}};
        return FS.createObject(parent, name, properties, canRead, canWrite);
      },createPath:function (parent, path, canRead, canWrite) {
        var current = FS.findObject(parent);
        if (current === null) throw new Error('Invalid parent.');
        path = path.split('/').reverse();
        while (path.length) {
          var part = path.pop();
          if (!part) continue;
          if (!current.contents.hasOwnProperty(part)) {
            FS.createFolder(current, part, canRead, canWrite);
          }
          current = current.contents[part];
        }
        return current;
      },createFile:function (parent, name, properties, canRead, canWrite) {
        properties.isFolder = false;
        return FS.createObject(parent, name, properties, canRead, canWrite);
      },createDataFile:function (parent, name, data, canRead, canWrite) {
        if (typeof data === 'string') {
          var dataArray = new Array(data.length);
          for (var i = 0, len = data.length; i < len; ++i) dataArray[i] = data.charCodeAt(i);
          data = dataArray;
        }
        var properties = {
          isDevice: false,
          contents: data.subarray ? data.subarray(0) : data // as an optimization, create a new array wrapper (not buffer) here, to help JS engines understand this object
        };
        return FS.createFile(parent, name, properties, canRead, canWrite);
      },createLazyFile:function (parent, name, url, canRead, canWrite) {
        if (typeof XMLHttpRequest !== 'undefined') {
          if (!ENVIRONMENT_IS_WORKER) throw 'Cannot do synchronous binary XHRs outside webworkers in modern browsers. Use --embed-file or --preload-file in emcc';
          // Lazy chunked Uint8Array (implements get and length from Uint8Array). Actual getting is abstracted away for eventual reuse.
          var LazyUint8Array = function() {
            this.lengthKnown = false;
            this.chunks = []; // Loaded chunks. Index is the chunk number
          }
          LazyUint8Array.prototype.get = function(idx) {
            if (idx > this.length-1 || idx < 0) {
              return undefined;
            }
            var chunkOffset = idx % this.chunkSize;
            var chunkNum = Math.floor(idx / this.chunkSize);
            return this.getter(chunkNum)[chunkOffset];
          }
          LazyUint8Array.prototype.setDataGetter = function(getter) {
            this.getter = getter;
          }
          LazyUint8Array.prototype.cacheLength = function() {
              // Find length
              var xhr = new XMLHttpRequest();
              xhr.open('HEAD', url, false);
              xhr.send(null);
              if (!(xhr.status >= 200 && xhr.status < 300 || xhr.status === 304)) throw new Error("Couldn't load " + url + ". Status: " + xhr.status);
              var datalength = Number(xhr.getResponseHeader("Content-length"));
              var header;
              var hasByteServing = (header = xhr.getResponseHeader("Accept-Ranges")) && header === "bytes";
              var chunkSize = 1024*1024; // Chunk size in bytes
              if (!hasByteServing) chunkSize = datalength;
              // Function to get a range from the remote URL.
              var doXHR = (function(from, to) {
                if (from > to) throw new Error("invalid range (" + from + ", " + to + ") or no bytes requested!");
                if (to > datalength-1) throw new Error("only " + datalength + " bytes available! programmer error!");
                // TODO: Use mozResponseArrayBuffer, responseStream, etc. if available.
                var xhr = new XMLHttpRequest();
                xhr.open('GET', url, false);
                if (datalength !== chunkSize) xhr.setRequestHeader("Range", "bytes=" + from + "-" + to);
                // Some hints to the browser that we want binary data.
                if (typeof Uint8Array != 'undefined') xhr.responseType = 'arraybuffer';
                if (xhr.overrideMimeType) {
                  xhr.overrideMimeType('text/plain; charset=x-user-defined');
                }
                xhr.send(null);
                if (!(xhr.status >= 200 && xhr.status < 300 || xhr.status === 304)) throw new Error("Couldn't load " + url + ". Status: " + xhr.status);
                if (xhr.response !== undefined) {
                  return new Uint8Array(xhr.response || []);
                } else {
                  return intArrayFromString(xhr.responseText || '', true);
                }
              });
              var lazyArray = this;
              lazyArray.setDataGetter(function(chunkNum) {
                var start = chunkNum * chunkSize;
                var end = (chunkNum+1) * chunkSize - 1; // including this byte
                end = Math.min(end, datalength-1); // if datalength-1 is selected, this is the last block
                if (typeof(lazyArray.chunks[chunkNum]) === "undefined") {
                  lazyArray.chunks[chunkNum] = doXHR(start, end);
                }
                if (typeof(lazyArray.chunks[chunkNum]) === "undefined") throw new Error("doXHR failed!");
                return lazyArray.chunks[chunkNum];
              });
              this._length = datalength;
              this._chunkSize = chunkSize;
              this.lengthKnown = true;
          }
          var lazyArray = new LazyUint8Array();
          Object.defineProperty(lazyArray, "length", {
              get: function() {
                  if(!this.lengthKnown) {
                      this.cacheLength();
                  }
                  return this._length;
              }
          });
          Object.defineProperty(lazyArray, "chunkSize", {
              get: function() {
                  if(!this.lengthKnown) {
                      this.cacheLength();
                  }
                  return this._chunkSize;
              }
          });
          var properties = { isDevice: false, contents: lazyArray };
        } else {
          var properties = { isDevice: false, url: url };
        }
        return FS.createFile(parent, name, properties, canRead, canWrite);
      },createPreloadedFile:function (parent, name, url, canRead, canWrite, onload, onerror, dontCreateFile) {
        Browser.init();
        var fullname = FS.joinPath([parent, name], true);
        function processData(byteArray) {
          function finish(byteArray) {
            if (!dontCreateFile) {
              FS.createDataFile(parent, name, byteArray, canRead, canWrite);
            }
            if (onload) onload();
            removeRunDependency('cp ' + fullname);
          }
          var handled = false;
          Module['preloadPlugins'].forEach(function(plugin) {
            if (handled) return;
            if (plugin['canHandle'](fullname)) {
              plugin['handle'](byteArray, fullname, finish, function() {
                if (onerror) onerror();
                removeRunDependency('cp ' + fullname);
              });
              handled = true;
            }
          });
          if (!handled) finish(byteArray);
        }
        addRunDependency('cp ' + fullname);
        if (typeof url == 'string') {
          Browser.asyncLoad(url, function(byteArray) {
            processData(byteArray);
          }, onerror);
        } else {
          processData(url);
        }
      },createLink:function (parent, name, target, canRead, canWrite) {
        var properties = {isDevice: false, link: target};
        return FS.createFile(parent, name, properties, canRead, canWrite);
      },createDevice:function (parent, name, input, output) {
        if (!(input || output)) {
          throw new Error('A device must have at least one callback defined.');
        }
        var ops = {isDevice: true, input: input, output: output};
        return FS.createFile(parent, name, ops, Boolean(input), Boolean(output));
      },forceLoadFile:function (obj) {
        if (obj.isDevice || obj.isFolder || obj.link || obj.contents) return true;
        var success = true;
        if (typeof XMLHttpRequest !== 'undefined') {
          throw new Error("Lazy loading should have been performed (contents set) in createLazyFile, but it was not. Lazy loading only works in web workers. Use --embed-file or --preload-file in emcc on the main thread.");
        } else if (Module['read']) {
          // Command-line.
          try {
            // WARNING: Can't read binary files in V8's d8 or tracemonkey's js, as
            //          read() will try to parse UTF8.
            obj.contents = intArrayFromString(Module['read'](obj.url), true);
          } catch (e) {
            success = false;
          }
        } else {
          throw new Error('Cannot load without read() or XMLHttpRequest.');
        }
        if (!success) ___setErrNo(ERRNO_CODES.EIO);
        return success;
      },ensureRoot:function () {
        if (FS.root) return;
        // The main file system tree. All the contents are inside this.
        FS.root = {
          read: true,
          write: true,
          isFolder: true,
          isDevice: false,
          timestamp: Date.now(),
          inodeNumber: 1,
          contents: {}
        };
      },init:function (input, output, error) {
        // Make sure we initialize only once.
        assert(!FS.init.initialized, 'FS.init was previously called. If you want to initialize later with custom parameters, remove any earlier calls (note that one is automatically added to the generated code)');
        FS.init.initialized = true;
        FS.ensureRoot();
        // Allow Module.stdin etc. to provide defaults, if none explicitly passed to us here
        input = input || Module['stdin'];
        output = output || Module['stdout'];
        error = error || Module['stderr'];
        // Default handlers.
        var stdinOverridden = true, stdoutOverridden = true, stderrOverridden = true;
        if (!input) {
          stdinOverridden = false;
          input = function() {
            if (!input.cache || !input.cache.length) {
              var result;
              if (typeof window != 'undefined' &&
                  typeof window.prompt == 'function') {
                // Browser.
                result = window.prompt('Input: ');
                if (result === null) result = String.fromCharCode(0); // cancel ==> EOF
              } else if (typeof readline == 'function') {
                // Command line.
                result = readline();
              }
              if (!result) result = '';
              input.cache = intArrayFromString(result + '\n', true);
            }
            return input.cache.shift();
          };
        }
        var utf8 = new Runtime.UTF8Processor();
        function simpleOutput(val) {
          if (val === null || val === 10) {
            output.printer(output.buffer.join(''));
            output.buffer = [];
          } else {
            output.buffer.push(utf8.processCChar(val));
          }
        }
        if (!output) {
          stdoutOverridden = false;
          output = simpleOutput;
        }
        if (!output.printer) output.printer = Module['print'];
        if (!output.buffer) output.buffer = [];
        if (!error) {
          stderrOverridden = false;
          error = simpleOutput;
        }
        if (!error.printer) error.printer = Module['print'];
        if (!error.buffer) error.buffer = [];
        // Create the temporary folder, if not already created
        try {
          FS.createFolder('/', 'tmp', true, true);
        } catch(e) {}
        // Create the I/O devices.
        var devFolder = FS.createFolder('/', 'dev', true, true);
        var stdin = FS.createDevice(devFolder, 'stdin', input);
        var stdout = FS.createDevice(devFolder, 'stdout', null, output);
        var stderr = FS.createDevice(devFolder, 'stderr', null, error);
        FS.createDevice(devFolder, 'tty', input, output);
        FS.createDevice(devFolder, 'null', function(){}, function(){});
        // Create default streams.
        FS.streams[1] = {
          path: '/dev/stdin',
          object: stdin,
          position: 0,
          isRead: true,
          isWrite: false,
          isAppend: false,
          isTerminal: !stdinOverridden,
          error: false,
          eof: false,
          ungotten: []
        };
        FS.streams[2] = {
          path: '/dev/stdout',
          object: stdout,
          position: 0,
          isRead: false,
          isWrite: true,
          isAppend: false,
          isTerminal: !stdoutOverridden,
          error: false,
          eof: false,
          ungotten: []
        };
        FS.streams[3] = {
          path: '/dev/stderr',
          object: stderr,
          position: 0,
          isRead: false,
          isWrite: true,
          isAppend: false,
          isTerminal: !stderrOverridden,
          error: false,
          eof: false,
          ungotten: []
        };
        // TODO: put these low in memory like we used to assert on: assert(Math.max(_stdin, _stdout, _stderr) < 15000); // make sure these are low, we flatten arrays with these
        HEAP32[((_stdin)>>2)]=1;
        HEAP32[((_stdout)>>2)]=2;
        HEAP32[((_stderr)>>2)]=3;
        // Other system paths
        FS.createPath('/', 'dev/shm/tmp', true, true); // temp files
        // Newlib initialization
        for (var i = FS.streams.length; i < Math.max(_stdin, _stdout, _stderr) + 4; i++) {
          FS.streams[i] = null; // Make sure to keep FS.streams dense
        }
        FS.streams[_stdin] = FS.streams[1];
        FS.streams[_stdout] = FS.streams[2];
        FS.streams[_stderr] = FS.streams[3];
        FS.checkStreams();
        // see previous TODO on stdin etc.: assert(FS.streams.length < 1024); // at this early stage, we should not have a large set of file descriptors - just a few
        allocate([ allocate(
          [0, 0, 0, 0, _stdin, 0, 0, 0, _stdout, 0, 0, 0, _stderr, 0, 0, 0],
          'void*', ALLOC_NORMAL) ], 'void*', ALLOC_NONE, __impure_ptr);
      },quit:function () {
        if (!FS.init.initialized) return;
        // Flush any partially-printed lines in stdout and stderr. Careful, they may have been closed
        if (FS.streams[2] && FS.streams[2].object.output.buffer.length > 0) FS.streams[2].object.output(10);
        if (FS.streams[3] && FS.streams[3].object.output.buffer.length > 0) FS.streams[3].object.output(10);
      },standardizePath:function (path) {
        if (path.substr(0, 2) == './') path = path.substr(2);
        return path;
      },deleteFile:function (path) {
        path = FS.analyzePath(path);
        if (!path.parentExists || !path.exists) {
          throw 'Invalid path ' + path;
        }
        delete path.parentObject.contents[path.name];
      }};
  function _send(fd, buf, len, flags) {
      var info = FS.streams[fd];
      if (!info) return -1;
      info.sender(HEAPU8.subarray(buf, buf+len));
      return len;
    }
  function _pwrite(fildes, buf, nbyte, offset) {
      // ssize_t pwrite(int fildes, const void *buf, size_t nbyte, off_t offset);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/write.html
      var stream = FS.streams[fildes];
      if (!stream || stream.object.isDevice) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      } else if (!stream.isWrite) {
        ___setErrNo(ERRNO_CODES.EACCES);
        return -1;
      } else if (stream.object.isFolder) {
        ___setErrNo(ERRNO_CODES.EISDIR);
        return -1;
      } else if (nbyte < 0 || offset < 0) {
        ___setErrNo(ERRNO_CODES.EINVAL);
        return -1;
      } else {
        var contents = stream.object.contents;
        while (contents.length < offset) contents.push(0);
        for (var i = 0; i < nbyte; i++) {
          contents[offset + i] = HEAPU8[(((buf)+(i))|0)];
        }
        stream.object.timestamp = Date.now();
        return i;
      }
    }function _write(fildes, buf, nbyte) {
      // ssize_t write(int fildes, const void *buf, size_t nbyte);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/write.html
      var stream = FS.streams[fildes];
      if (stream && ('socket' in stream)) {
          return _send(fildes, buf, nbyte, 0);
      } else if (!stream) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      } else if (!stream.isWrite) {
        ___setErrNo(ERRNO_CODES.EACCES);
        return -1;
      } else if (nbyte < 0) {
        ___setErrNo(ERRNO_CODES.EINVAL);
        return -1;
      } else {
        if (stream.object.isDevice) {
          if (stream.object.output) {
            for (var i = 0; i < nbyte; i++) {
              try {
                stream.object.output(HEAP8[(((buf)+(i))|0)]);
              } catch (e) {
                ___setErrNo(ERRNO_CODES.EIO);
                return -1;
              }
            }
            stream.object.timestamp = Date.now();
            return i;
          } else {
            ___setErrNo(ERRNO_CODES.ENXIO);
            return -1;
          }
        } else {
          var bytesWritten = _pwrite(fildes, buf, nbyte, stream.position);
          if (bytesWritten != -1) stream.position += bytesWritten;
          return bytesWritten;
        }
      }
    }function _fwrite(ptr, size, nitems, stream) {
      // size_t fwrite(const void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/fwrite.html
      var bytesToWrite = nitems * size;
      if (bytesToWrite == 0) return 0;
      var bytesWritten = _write(stream, ptr, bytesToWrite);
      if (bytesWritten == -1) {
        if (FS.streams[stream]) FS.streams[stream].error = true;
        return 0;
      } else {
        return Math.floor(bytesWritten / size);
      }
    }
  function _strlen(ptr) {
      ptr = ptr|0;
      var curr = 0;
      curr = ptr;
      while (HEAP8[(curr)]) {
        curr = (curr + 1)|0;
      }
      return (curr - ptr)|0;
    }
  function __reallyNegative(x) {
      return x < 0 || (x === 0 && (1/x) === -Infinity);
    }function __formatString(format, varargs) {
      var textIndex = format;
      var argIndex = 0;
      function getNextArg(type) {
        // NOTE: Explicitly ignoring type safety. Otherwise this fails:
        //       int x = 4; printf("%c\n", (char)x);
        var ret;
        if (type === 'double') {
          ret = HEAPF64[(((varargs)+(argIndex))>>3)];
        } else if (type == 'i64') {
          ret = [HEAP32[(((varargs)+(argIndex))>>2)],
                 HEAP32[(((varargs)+(argIndex+8))>>2)]];
          argIndex += 8; // each 32-bit chunk is in a 64-bit block
        } else {
          type = 'i32'; // varargs are always i32, i64, or double
          ret = HEAP32[(((varargs)+(argIndex))>>2)];
        }
        argIndex += Math.max(Runtime.getNativeFieldSize(type), Runtime.getAlignSize(type, null, true));
        return ret;
      }
      var ret = [];
      var curr, next, currArg;
      while(1) {
        var startTextIndex = textIndex;
        curr = HEAP8[(textIndex)];
        if (curr === 0) break;
        next = HEAP8[((textIndex+1)|0)];
        if (curr == 37) {
          // Handle flags.
          var flagAlwaysSigned = false;
          var flagLeftAlign = false;
          var flagAlternative = false;
          var flagZeroPad = false;
          flagsLoop: while (1) {
            switch (next) {
              case 43:
                flagAlwaysSigned = true;
                break;
              case 45:
                flagLeftAlign = true;
                break;
              case 35:
                flagAlternative = true;
                break;
              case 48:
                if (flagZeroPad) {
                  break flagsLoop;
                } else {
                  flagZeroPad = true;
                  break;
                }
              default:
                break flagsLoop;
            }
            textIndex++;
            next = HEAP8[((textIndex+1)|0)];
          }
          // Handle width.
          var width = 0;
          if (next == 42) {
            width = getNextArg('i32');
            textIndex++;
            next = HEAP8[((textIndex+1)|0)];
          } else {
            while (next >= 48 && next <= 57) {
              width = width * 10 + (next - 48);
              textIndex++;
              next = HEAP8[((textIndex+1)|0)];
            }
          }
          // Handle precision.
          var precisionSet = false;
          if (next == 46) {
            var precision = 0;
            precisionSet = true;
            textIndex++;
            next = HEAP8[((textIndex+1)|0)];
            if (next == 42) {
              precision = getNextArg('i32');
              textIndex++;
            } else {
              while(1) {
                var precisionChr = HEAP8[((textIndex+1)|0)];
                if (precisionChr < 48 ||
                    precisionChr > 57) break;
                precision = precision * 10 + (precisionChr - 48);
                textIndex++;
              }
            }
            next = HEAP8[((textIndex+1)|0)];
          } else {
            var precision = 6; // Standard default.
          }
          // Handle integer sizes. WARNING: These assume a 32-bit architecture!
          var argSize;
          switch (String.fromCharCode(next)) {
            case 'h':
              var nextNext = HEAP8[((textIndex+2)|0)];
              if (nextNext == 104) {
                textIndex++;
                argSize = 1; // char (actually i32 in varargs)
              } else {
                argSize = 2; // short (actually i32 in varargs)
              }
              break;
            case 'l':
              var nextNext = HEAP8[((textIndex+2)|0)];
              if (nextNext == 108) {
                textIndex++;
                argSize = 8; // long long
              } else {
                argSize = 4; // long
              }
              break;
            case 'L': // long long
            case 'q': // int64_t
            case 'j': // intmax_t
              argSize = 8;
              break;
            case 'z': // size_t
            case 't': // ptrdiff_t
            case 'I': // signed ptrdiff_t or unsigned size_t
              argSize = 4;
              break;
            default:
              argSize = null;
          }
          if (argSize) textIndex++;
          next = HEAP8[((textIndex+1)|0)];
          // Handle type specifier.
          switch (String.fromCharCode(next)) {
            case 'd': case 'i': case 'u': case 'o': case 'x': case 'X': case 'p': {
              // Integer.
              var signed = next == 100 || next == 105;
              argSize = argSize || 4;
              var currArg = getNextArg('i' + (argSize * 8));
              var origArg = currArg;
              var argText;
              // Flatten i64-1 [low, high] into a (slightly rounded) double
              if (argSize == 8) {
                currArg = Runtime.makeBigInt(currArg[0], currArg[1], next == 117);
              }
              // Truncate to requested size.
              if (argSize <= 4) {
                var limit = Math.pow(256, argSize) - 1;
                currArg = (signed ? reSign : unSign)(currArg & limit, argSize * 8);
              }
              // Format the number.
              var currAbsArg = Math.abs(currArg);
              var prefix = '';
              if (next == 100 || next == 105) {
                if (argSize == 8 && i64Math) argText = i64Math.stringify(origArg[0], origArg[1], null); else
                argText = reSign(currArg, 8 * argSize, 1).toString(10);
              } else if (next == 117) {
                if (argSize == 8 && i64Math) argText = i64Math.stringify(origArg[0], origArg[1], true); else
                argText = unSign(currArg, 8 * argSize, 1).toString(10);
                currArg = Math.abs(currArg);
              } else if (next == 111) {
                argText = (flagAlternative ? '0' : '') + currAbsArg.toString(8);
              } else if (next == 120 || next == 88) {
                prefix = (flagAlternative && currArg != 0) ? '0x' : '';
                if (argSize == 8 && i64Math) {
                  if (origArg[1]) {
                    argText = (origArg[1]>>>0).toString(16);
                    var lower = (origArg[0]>>>0).toString(16);
                    while (lower.length < 8) lower = '0' + lower;
                    argText += lower;
                  } else {
                    argText = (origArg[0]>>>0).toString(16);
                  }
                } else
                if (currArg < 0) {
                  // Represent negative numbers in hex as 2's complement.
                  currArg = -currArg;
                  argText = (currAbsArg - 1).toString(16);
                  var buffer = [];
                  for (var i = 0; i < argText.length; i++) {
                    buffer.push((0xF - parseInt(argText[i], 16)).toString(16));
                  }
                  argText = buffer.join('');
                  while (argText.length < argSize * 2) argText = 'f' + argText;
                } else {
                  argText = currAbsArg.toString(16);
                }
                if (next == 88) {
                  prefix = prefix.toUpperCase();
                  argText = argText.toUpperCase();
                }
              } else if (next == 112) {
                if (currAbsArg === 0) {
                  argText = '(nil)';
                } else {
                  prefix = '0x';
                  argText = currAbsArg.toString(16);
                }
              }
              if (precisionSet) {
                while (argText.length < precision) {
                  argText = '0' + argText;
                }
              }
              // Add sign if needed
              if (flagAlwaysSigned) {
                if (currArg < 0) {
                  prefix = '-' + prefix;
                } else {
                  prefix = '+' + prefix;
                }
              }
              // Add padding.
              while (prefix.length + argText.length < width) {
                if (flagLeftAlign) {
                  argText += ' ';
                } else {
                  if (flagZeroPad) {
                    argText = '0' + argText;
                  } else {
                    prefix = ' ' + prefix;
                  }
                }
              }
              // Insert the result into the buffer.
              argText = prefix + argText;
              argText.split('').forEach(function(chr) {
                ret.push(chr.charCodeAt(0));
              });
              break;
            }
            case 'f': case 'F': case 'e': case 'E': case 'g': case 'G': {
              // Float.
              var currArg = getNextArg('double');
              var argText;
              if (isNaN(currArg)) {
                argText = 'nan';
                flagZeroPad = false;
              } else if (!isFinite(currArg)) {
                argText = (currArg < 0 ? '-' : '') + 'inf';
                flagZeroPad = false;
              } else {
                var isGeneral = false;
                var effectivePrecision = Math.min(precision, 20);
                // Convert g/G to f/F or e/E, as per:
                // http://pubs.opengroup.org/onlinepubs/9699919799/functions/printf.html
                if (next == 103 || next == 71) {
                  isGeneral = true;
                  precision = precision || 1;
                  var exponent = parseInt(currArg.toExponential(effectivePrecision).split('e')[1], 10);
                  if (precision > exponent && exponent >= -4) {
                    next = ((next == 103) ? 'f' : 'F').charCodeAt(0);
                    precision -= exponent + 1;
                  } else {
                    next = ((next == 103) ? 'e' : 'E').charCodeAt(0);
                    precision--;
                  }
                  effectivePrecision = Math.min(precision, 20);
                }
                if (next == 101 || next == 69) {
                  argText = currArg.toExponential(effectivePrecision);
                  // Make sure the exponent has at least 2 digits.
                  if (/[eE][-+]\d$/.test(argText)) {
                    argText = argText.slice(0, -1) + '0' + argText.slice(-1);
                  }
                } else if (next == 102 || next == 70) {
                  argText = currArg.toFixed(effectivePrecision);
                  if (currArg === 0 && __reallyNegative(currArg)) {
                    argText = '-' + argText;
                  }
                }
                var parts = argText.split('e');
                if (isGeneral && !flagAlternative) {
                  // Discard trailing zeros and periods.
                  while (parts[0].length > 1 && parts[0].indexOf('.') != -1 &&
                         (parts[0].slice(-1) == '0' || parts[0].slice(-1) == '.')) {
                    parts[0] = parts[0].slice(0, -1);
                  }
                } else {
                  // Make sure we have a period in alternative mode.
                  if (flagAlternative && argText.indexOf('.') == -1) parts[0] += '.';
                  // Zero pad until required precision.
                  while (precision > effectivePrecision++) parts[0] += '0';
                }
                argText = parts[0] + (parts.length > 1 ? 'e' + parts[1] : '');
                // Capitalize 'E' if needed.
                if (next == 69) argText = argText.toUpperCase();
                // Add sign.
                if (flagAlwaysSigned && currArg >= 0) {
                  argText = '+' + argText;
                }
              }
              // Add padding.
              while (argText.length < width) {
                if (flagLeftAlign) {
                  argText += ' ';
                } else {
                  if (flagZeroPad && (argText[0] == '-' || argText[0] == '+')) {
                    argText = argText[0] + '0' + argText.slice(1);
                  } else {
                    argText = (flagZeroPad ? '0' : ' ') + argText;
                  }
                }
              }
              // Adjust case.
              if (next < 97) argText = argText.toUpperCase();
              // Insert the result into the buffer.
              argText.split('').forEach(function(chr) {
                ret.push(chr.charCodeAt(0));
              });
              break;
            }
            case 's': {
              // String.
              var arg = getNextArg('i8*');
              var argLength = arg ? _strlen(arg) : '(null)'.length;
              if (precisionSet) argLength = Math.min(argLength, precision);
              if (!flagLeftAlign) {
                while (argLength < width--) {
                  ret.push(32);
                }
              }
              if (arg) {
                for (var i = 0; i < argLength; i++) {
                  ret.push(HEAPU8[((arg++)|0)]);
                }
              } else {
                ret = ret.concat(intArrayFromString('(null)'.substr(0, argLength), true));
              }
              if (flagLeftAlign) {
                while (argLength < width--) {
                  ret.push(32);
                }
              }
              break;
            }
            case 'c': {
              // Character.
              if (flagLeftAlign) ret.push(getNextArg('i8'));
              while (--width > 0) {
                ret.push(32);
              }
              if (!flagLeftAlign) ret.push(getNextArg('i8'));
              break;
            }
            case 'n': {
              // Write the length written so far to the next parameter.
              var ptr = getNextArg('i32*');
              HEAP32[((ptr)>>2)]=ret.length
              break;
            }
            case '%': {
              // Literal percent sign.
              ret.push(curr);
              break;
            }
            default: {
              // Unknown specifiers remain untouched.
              for (var i = startTextIndex; i < textIndex + 2; i++) {
                ret.push(HEAP8[(i)]);
              }
            }
          }
          textIndex += 2;
          // TODO: Support a/A (hex float) and m (last error) specifiers.
          // TODO: Support %1${specifier} for arg selection.
        } else {
          ret.push(curr);
          textIndex += 1;
        }
      }
      return ret;
    }function _fprintf(stream, format, varargs) {
      // int fprintf(FILE *restrict stream, const char *restrict format, ...);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/printf.html
      var result = __formatString(format, varargs);
      var stack = Runtime.stackSave();
      var ret = _fwrite(allocate(result, 'i8', ALLOC_STACK), 1, result.length, stream);
      Runtime.stackRestore(stack);
      return ret;
    }function _printf(format, varargs) {
      // int printf(const char *restrict format, ...);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/printf.html
      var stdout = HEAP32[((_stdout)>>2)];
      return _fprintf(stdout, format, varargs);
    }function _vprintf(format, va_arg) {
      return _printf(format, HEAP32[((va_arg)>>2)]);
    }
  function _llvm_va_end() {}
  function ___gxx_personality_v0() {
    }
  function __exit(status) {
      // void _exit(int status);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/exit.html
      function ExitStatus() {
        this.name = "ExitStatus";
        this.message = "Program terminated with exit(" + status + ")";
        this.status = status;
        Module.print('Exit Status: ' + status);
      };
      ExitStatus.prototype = new Error();
      ExitStatus.prototype.constructor = ExitStatus;
      exitRuntime();
      ABORT = true;
      throw new ExitStatus();
    }function _exit(status) {
      __exit(status);
    }function __ZSt9terminatev() {
      _exit(-1234);
    }
  function _atexit(func, arg) {
      __ATEXIT__.unshift({ func: func, arg: arg });
    }var ___cxa_atexit=_atexit;
  var Browser={mainLoop:{scheduler:null,shouldPause:false,paused:false,queue:[],pause:function () {
          Browser.mainLoop.shouldPause = true;
        },resume:function () {
          if (Browser.mainLoop.paused) {
            Browser.mainLoop.paused = false;
            Browser.mainLoop.scheduler();
          }
          Browser.mainLoop.shouldPause = false;
        },updateStatus:function () {
          if (Module['setStatus']) {
            var message = Module['statusMessage'] || 'Please wait...';
            var remaining = Browser.mainLoop.remainingBlockers;
            var expected = Browser.mainLoop.expectedBlockers;
            if (remaining) {
              if (remaining < expected) {
                Module['setStatus'](message + ' (' + (expected - remaining) + '/' + expected + ')');
              } else {
                Module['setStatus'](message);
              }
            } else {
              Module['setStatus']('');
            }
          }
        }},isFullScreen:false,pointerLock:false,moduleContextCreatedCallbacks:[],workers:[],init:function () {
        if (Browser.initted) return;
        Browser.initted = true;
        try {
          new Blob();
          Browser.hasBlobConstructor = true;
        } catch(e) {
          Browser.hasBlobConstructor = false;
          console.log("warning: no blob constructor, cannot create blobs with mimetypes");
        }
        Browser.BlobBuilder = typeof MozBlobBuilder != "undefined" ? MozBlobBuilder : (typeof WebKitBlobBuilder != "undefined" ? WebKitBlobBuilder : (!Browser.hasBlobConstructor ? console.log("warning: no BlobBuilder") : null));
        Browser.URLObject = typeof window != "undefined" ? (window.URL ? window.URL : window.webkitURL) : console.log("warning: cannot create object URLs");
        // Support for plugins that can process preloaded files. You can add more of these to
        // your app by creating and appending to Module.preloadPlugins.
        //
        // Each plugin is asked if it can handle a file based on the file's name. If it can,
        // it is given the file's raw data. When it is done, it calls a callback with the file's
        // (possibly modified) data. For example, a plugin might decompress a file, or it
        // might create some side data structure for use later (like an Image element, etc.).
        function getMimetype(name) {
          return {
            'jpg': 'image/jpeg',
            'jpeg': 'image/jpeg',
            'png': 'image/png',
            'bmp': 'image/bmp',
            'ogg': 'audio/ogg',
            'wav': 'audio/wav',
            'mp3': 'audio/mpeg'
          }[name.substr(name.lastIndexOf('.')+1)];
        }
        if (!Module["preloadPlugins"]) Module["preloadPlugins"] = [];
        var imagePlugin = {};
        imagePlugin['canHandle'] = function(name) {
          return !Module.noImageDecoding && /\.(jpg|jpeg|png|bmp)$/.exec(name);
        };
        imagePlugin['handle'] = function(byteArray, name, onload, onerror) {
          var b = null;
          if (Browser.hasBlobConstructor) {
            try {
              b = new Blob([byteArray], { type: getMimetype(name) });
            } catch(e) {
              Runtime.warnOnce('Blob constructor present but fails: ' + e + '; falling back to blob builder');
            }
          }
          if (!b) {
            var bb = new Browser.BlobBuilder();
            bb.append((new Uint8Array(byteArray)).buffer); // we need to pass a buffer, and must copy the array to get the right data range
            b = bb.getBlob();
          }
          var url = Browser.URLObject.createObjectURL(b);
          assert(typeof url == 'string', 'createObjectURL must return a url as a string');
          var img = new Image();
          img.onload = function() {
            assert(img.complete, 'Image ' + name + ' could not be decoded');
            var canvas = document.createElement('canvas');
            canvas.width = img.width;
            canvas.height = img.height;
            var ctx = canvas.getContext('2d');
            ctx.drawImage(img, 0, 0);
            Module["preloadedImages"][name] = canvas;
            Browser.URLObject.revokeObjectURL(url);
            if (onload) onload(byteArray);
          };
          img.onerror = function(event) {
            console.log('Image ' + url + ' could not be decoded');
            if (onerror) onerror();
          };
          img.src = url;
        };
        Module['preloadPlugins'].push(imagePlugin);
        var audioPlugin = {};
        audioPlugin['canHandle'] = function(name) {
          return !Module.noAudioDecoding && name.substr(-4) in { '.ogg': 1, '.wav': 1, '.mp3': 1 };
        };
        audioPlugin['handle'] = function(byteArray, name, onload, onerror) {
          var done = false;
          function finish(audio) {
            if (done) return;
            done = true;
            Module["preloadedAudios"][name] = audio;
            if (onload) onload(byteArray);
          }
          function fail() {
            if (done) return;
            done = true;
            Module["preloadedAudios"][name] = new Audio(); // empty shim
            if (onerror) onerror();
          }
          if (Browser.hasBlobConstructor) {
            try {
              var b = new Blob([byteArray], { type: getMimetype(name) });
            } catch(e) {
              return fail();
            }
            var url = Browser.URLObject.createObjectURL(b); // XXX we never revoke this!
            assert(typeof url == 'string', 'createObjectURL must return a url as a string');
            var audio = new Audio();
            audio.addEventListener('canplaythrough', function() { finish(audio) }, false); // use addEventListener due to chromium bug 124926
            audio.onerror = function(event) {
              if (done) return;
              console.log('warning: browser could not fully decode audio ' + name + ', trying slower base64 approach');
              function encode64(data) {
                var BASE = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
                var PAD = '=';
                var ret = '';
                var leftchar = 0;
                var leftbits = 0;
                for (var i = 0; i < data.length; i++) {
                  leftchar = (leftchar << 8) | data[i];
                  leftbits += 8;
                  while (leftbits >= 6) {
                    var curr = (leftchar >> (leftbits-6)) & 0x3f;
                    leftbits -= 6;
                    ret += BASE[curr];
                  }
                }
                if (leftbits == 2) {
                  ret += BASE[(leftchar&3) << 4];
                  ret += PAD + PAD;
                } else if (leftbits == 4) {
                  ret += BASE[(leftchar&0xf) << 2];
                  ret += PAD;
                }
                return ret;
              }
              audio.src = 'data:audio/x-' + name.substr(-3) + ';base64,' + encode64(byteArray);
              finish(audio); // we don't wait for confirmation this worked - but it's worth trying
            };
            audio.src = url;
            // workaround for chrome bug 124926 - we do not always get oncanplaythrough or onerror
            Browser.safeSetTimeout(function() {
              finish(audio); // try to use it even though it is not necessarily ready to play
            }, 10000);
          } else {
            return fail();
          }
        };
        Module['preloadPlugins'].push(audioPlugin);
        // Canvas event setup
        var canvas = Module['canvas'];
        canvas.requestPointerLock = canvas['requestPointerLock'] ||
                                    canvas['mozRequestPointerLock'] ||
                                    canvas['webkitRequestPointerLock'];
        canvas.exitPointerLock = document['exitPointerLock'] ||
                                 document['mozExitPointerLock'] ||
                                 document['webkitExitPointerLock'] ||
                                 function(){}; // no-op if function does not exist
        canvas.exitPointerLock = canvas.exitPointerLock.bind(document);
        function pointerLockChange() {
          Browser.pointerLock = document['pointerLockElement'] === canvas ||
                                document['mozPointerLockElement'] === canvas ||
                                document['webkitPointerLockElement'] === canvas;
        }
        document.addEventListener('pointerlockchange', pointerLockChange, false);
        document.addEventListener('mozpointerlockchange', pointerLockChange, false);
        document.addEventListener('webkitpointerlockchange', pointerLockChange, false);
        if (Module['elementPointerLock']) {
          canvas.addEventListener("click", function(ev) {
            if (!Browser.pointerLock && canvas.requestPointerLock) {
              canvas.requestPointerLock();
              ev.preventDefault();
            }
          }, false);
        }
      },createContext:function (canvas, useWebGL, setInModule) {
        var ctx;
        try {
          if (useWebGL) {
            ctx = canvas.getContext('experimental-webgl', {
              alpha: false
            });
          } else {
            ctx = canvas.getContext('2d');
          }
          if (!ctx) throw ':(';
        } catch (e) {
          Module.print('Could not create canvas - ' + e);
          return null;
        }
        if (useWebGL) {
          // Set the background of the WebGL canvas to black
          canvas.style.backgroundColor = "black";
          // Warn on context loss
          canvas.addEventListener('webglcontextlost', function(event) {
            alert('WebGL context lost. You will need to reload the page.');
          }, false);
        }
        if (setInModule) {
          Module.ctx = ctx;
          Module.useWebGL = useWebGL;
          Browser.moduleContextCreatedCallbacks.forEach(function(callback) { callback() });
          Browser.init();
        }
        return ctx;
      },destroyContext:function (canvas, useWebGL, setInModule) {},fullScreenHandlersInstalled:false,lockPointer:undefined,resizeCanvas:undefined,requestFullScreen:function (lockPointer, resizeCanvas) {
        Browser.lockPointer = lockPointer;
        Browser.resizeCanvas = resizeCanvas;
        if (typeof Browser.lockPointer === 'undefined') Browser.lockPointer = true;
        if (typeof Browser.resizeCanvas === 'undefined') Browser.resizeCanvas = false;
        var canvas = Module['canvas'];
        function fullScreenChange() {
          Browser.isFullScreen = false;
          if ((document['webkitFullScreenElement'] || document['webkitFullscreenElement'] ||
               document['mozFullScreenElement'] || document['mozFullscreenElement'] ||
               document['fullScreenElement'] || document['fullscreenElement']) === canvas) {
            canvas.cancelFullScreen = document['cancelFullScreen'] ||
                                      document['mozCancelFullScreen'] ||
                                      document['webkitCancelFullScreen'];
            canvas.cancelFullScreen = canvas.cancelFullScreen.bind(document);
            if (Browser.lockPointer) canvas.requestPointerLock();
            Browser.isFullScreen = true;
            if (Browser.resizeCanvas) Browser.setFullScreenCanvasSize();
          } else if (Browser.resizeCanvas){
            Browser.setWindowedCanvasSize();
          }
          if (Module['onFullScreen']) Module['onFullScreen'](Browser.isFullScreen);
        }
        if (!Browser.fullScreenHandlersInstalled) {
          Browser.fullScreenHandlersInstalled = true;
          document.addEventListener('fullscreenchange', fullScreenChange, false);
          document.addEventListener('mozfullscreenchange', fullScreenChange, false);
          document.addEventListener('webkitfullscreenchange', fullScreenChange, false);
        }
        canvas.requestFullScreen = canvas['requestFullScreen'] ||
                                   canvas['mozRequestFullScreen'] ||
                                   (canvas['webkitRequestFullScreen'] ? function() { canvas['webkitRequestFullScreen'](Element['ALLOW_KEYBOARD_INPUT']) } : null);
        canvas.requestFullScreen();
      },requestAnimationFrame:function (func) {
        if (!window.requestAnimationFrame) {
          window.requestAnimationFrame = window['requestAnimationFrame'] ||
                                         window['mozRequestAnimationFrame'] ||
                                         window['webkitRequestAnimationFrame'] ||
                                         window['msRequestAnimationFrame'] ||
                                         window['oRequestAnimationFrame'] ||
                                         window['setTimeout'];
        }
        window.requestAnimationFrame(func);
      },safeCallback:function (func) {
        return function() {
          if (!ABORT) return func.apply(null, arguments);
        };
      },safeRequestAnimationFrame:function (func) {
        return Browser.requestAnimationFrame(function() {
          if (!ABORT) func();
        });
      },safeSetTimeout:function (func, timeout) {
        return setTimeout(function() {
          if (!ABORT) func();
        }, timeout);
      },safeSetInterval:function (func, timeout) {
        return setInterval(function() {
          if (!ABORT) func();
        }, timeout);
      },getUserMedia:function (func) {
        if(!window.getUserMedia) {
          window.getUserMedia = navigator['getUserMedia'] ||
                                navigator['mozGetUserMedia'];
        }
        window.getUserMedia(func);
      },getMovementX:function (event) {
        return event['movementX'] ||
               event['mozMovementX'] ||
               event['webkitMovementX'] ||
               0;
      },getMovementY:function (event) {
        return event['movementY'] ||
               event['mozMovementY'] ||
               event['webkitMovementY'] ||
               0;
      },mouseX:0,mouseY:0,mouseMovementX:0,mouseMovementY:0,calculateMouseEvent:function (event) { // event should be mousemove, mousedown or mouseup
        if (Browser.pointerLock) {
          // When the pointer is locked, calculate the coordinates
          // based on the movement of the mouse.
          // Workaround for Firefox bug 764498
          if (event.type != 'mousemove' &&
              ('mozMovementX' in event)) {
            Browser.mouseMovementX = Browser.mouseMovementY = 0;
          } else {
            Browser.mouseMovementX = Browser.getMovementX(event);
            Browser.mouseMovementY = Browser.getMovementY(event);
          }
          Browser.mouseX = SDL.mouseX + Browser.mouseMovementX;
          Browser.mouseY = SDL.mouseY + Browser.mouseMovementY;
        } else {
          // Otherwise, calculate the movement based on the changes
          // in the coordinates.
          var rect = Module["canvas"].getBoundingClientRect();
          var x = event.pageX - (window.scrollX + rect.left);
          var y = event.pageY - (window.scrollY + rect.top);
          // the canvas might be CSS-scaled compared to its backbuffer;
          // SDL-using content will want mouse coordinates in terms
          // of backbuffer units.
          var cw = Module["canvas"].width;
          var ch = Module["canvas"].height;
          x = x * (cw / rect.width);
          y = y * (ch / rect.height);
          Browser.mouseMovementX = x - Browser.mouseX;
          Browser.mouseMovementY = y - Browser.mouseY;
          Browser.mouseX = x;
          Browser.mouseY = y;
        }
      },xhrLoad:function (url, onload, onerror) {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', url, true);
        xhr.responseType = 'arraybuffer';
        xhr.onload = function() {
          if (xhr.status == 200 || (xhr.status == 0 && xhr.response)) { // file URLs can return 0
            onload(xhr.response);
          } else {
            onerror();
          }
        };
        xhr.onerror = onerror;
        xhr.send(null);
      },asyncLoad:function (url, onload, onerror, noRunDep) {
        Browser.xhrLoad(url, function(arrayBuffer) {
          assert(arrayBuffer, 'Loading data file "' + url + '" failed (no arrayBuffer).');
          onload(new Uint8Array(arrayBuffer));
          if (!noRunDep) removeRunDependency('al ' + url);
        }, function(event) {
          if (onerror) {
            onerror();
          } else {
            throw 'Loading data file "' + url + '" failed.';
          }
        });
        if (!noRunDep) addRunDependency('al ' + url);
      },resizeListeners:[],updateResizeListeners:function () {
        var canvas = Module['canvas'];
        Browser.resizeListeners.forEach(function(listener) {
          listener(canvas.width, canvas.height);
        });
      },setCanvasSize:function (width, height, noUpdates) {
        var canvas = Module['canvas'];
        canvas.width = width;
        canvas.height = height;
        if (!noUpdates) Browser.updateResizeListeners();
      },windowedWidth:0,windowedHeight:0,setFullScreenCanvasSize:function () {
        var canvas = Module['canvas'];
        this.windowedWidth = canvas.width;
        this.windowedHeight = canvas.height;
        canvas.width = screen.width;
        canvas.height = screen.height;
        var flags = HEAPU32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)];
        flags = flags | 0x00800000; // set SDL_FULLSCREEN flag
        HEAP32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)]=flags
        Browser.updateResizeListeners();
      },setWindowedCanvasSize:function () {
        var canvas = Module['canvas'];
        canvas.width = this.windowedWidth;
        canvas.height = this.windowedHeight;
        var flags = HEAPU32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)];
        flags = flags & ~0x00800000; // clear SDL_FULLSCREEN flag
        HEAP32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)]=flags
        Browser.updateResizeListeners();
      }};var SDL={defaults:{width:320,height:200,copyOnLock:true},version:null,surfaces:{},events:[],fonts:[null],audios:[null],music:{audio:null,volume:1},mixerFrequency:22050,mixerFormat:32784,mixerNumChannels:2,mixerChunkSize:1024,channelMinimumNumber:0,GL:false,keyboardState:null,keyboardMap:{},textInput:false,startTime:null,buttonState:0,modState:0,DOMButtons:[0,0,0],DOMEventToSDLEvent:{},keyCodes:{16:1249,17:1248,18:1250,33:1099,34:1102,37:1104,38:1106,39:1103,40:1105,46:127,96:1112,97:1113,98:1114,99:1115,100:1116,101:1117,102:1118,103:1119,104:1120,105:1121,112:1082,113:1083,114:1084,115:1085,116:1086,117:1087,118:1088,119:1089,120:1090,121:1091,122:1092,123:1093,173:45,188:44,190:46,191:47,192:96},scanCodes:{9:43,13:40,27:41,32:44,44:54,46:55,47:56,48:39,49:30,50:31,51:32,52:33,53:34,54:35,55:36,56:37,57:38,92:49,97:4,98:5,99:6,100:7,101:8,102:9,103:10,104:11,105:12,106:13,107:14,108:15,109:16,110:17,111:18,112:19,113:20,114:21,115:22,116:23,117:24,118:25,119:26,120:27,121:28,122:29,305:224,308:226},structs:{Rect:{__size__:16,x:0,y:4,w:8,h:12},PixelFormat:{__size__:36,format:0,palette:4,BitsPerPixel:8,BytesPerPixel:9,padding1:10,padding2:11,Rmask:12,Gmask:16,Bmask:20,Amask:24,Rloss:28,Gloss:29,Bloss:30,Aloss:31,Rshift:32,Gshift:33,Bshift:34,Ashift:35},KeyboardEvent:{__size__:16,type:0,windowID:4,state:8,repeat:9,padding2:10,padding3:11,keysym:12},keysym:{__size__:16,scancode:0,sym:4,mod:8,unicode:12},TextInputEvent:{__size__:264,type:0,windowID:4,text:8},MouseMotionEvent:{__size__:28,type:0,windowID:4,state:8,padding1:9,padding2:10,padding3:11,x:12,y:16,xrel:20,yrel:24},MouseButtonEvent:{__size__:20,type:0,windowID:4,button:8,state:9,padding1:10,padding2:11,x:12,y:16},ResizeEvent:{__size__:12,type:0,w:4,h:8},AudioSpec:{__size__:24,freq:0,format:4,channels:6,silence:7,samples:8,size:12,callback:16,userdata:20},version:{__size__:3,major:0,minor:1,patch:2}},loadRect:function (rect) {
        return {
          x: HEAP32[((rect + SDL.structs.Rect.x)>>2)],
          y: HEAP32[((rect + SDL.structs.Rect.y)>>2)],
          w: HEAP32[((rect + SDL.structs.Rect.w)>>2)],
          h: HEAP32[((rect + SDL.structs.Rect.h)>>2)]
        };
      },loadColorToCSSRGB:function (color) {
        var rgba = HEAP32[((color)>>2)];
        return 'rgb(' + (rgba&255) + ',' + ((rgba >> 8)&255) + ',' + ((rgba >> 16)&255) + ')';
      },loadColorToCSSRGBA:function (color) {
        var rgba = HEAP32[((color)>>2)];
        return 'rgba(' + (rgba&255) + ',' + ((rgba >> 8)&255) + ',' + ((rgba >> 16)&255) + ',' + (((rgba >> 24)&255)/255) + ')';
      },translateColorToCSSRGBA:function (rgba) {
        return 'rgba(' + ((rgba >> 24)&255) + ',' + ((rgba >> 16)&255) + ',' + ((rgba >> 8)&255) + ',' + ((rgba&255)/255) + ')';
      },translateRGBAToCSSRGBA:function (r, g, b, a) {
        return 'rgba(' + r + ',' + g + ',' + b + ',' + (a/255) + ')';
      },translateRGBAToColor:function (r, g, b, a) {
        return (r << 24) + (g << 16) + (b << 8) + a;
      },makeSurface:function (width, height, flags, usePageCanvas, source, rmask, gmask, bmask, amask) {
        flags = flags || 0;
        var surf = _malloc(14*Runtime.QUANTUM_SIZE);  // SDL_Surface has 14 fields of quantum size
        var buffer = _malloc(width*height*4); // TODO: only allocate when locked the first time
        var pixelFormat = _malloc(18*Runtime.QUANTUM_SIZE);
        flags |= 1; // SDL_HWSURFACE - this tells SDL_MUSTLOCK that this needs to be locked
        //surface with SDL_HWPALETTE flag is 8bpp surface (1 byte)
        var is_SDL_HWPALETTE = flags & 0x00200000;  
        var bpp = is_SDL_HWPALETTE ? 1 : 4;
        HEAP32[((surf+Runtime.QUANTUM_SIZE*0)>>2)]=flags         // SDL_Surface.flags
        HEAP32[((surf+Runtime.QUANTUM_SIZE*1)>>2)]=pixelFormat // SDL_Surface.format TODO
        HEAP32[((surf+Runtime.QUANTUM_SIZE*2)>>2)]=width         // SDL_Surface.w
        HEAP32[((surf+Runtime.QUANTUM_SIZE*3)>>2)]=height        // SDL_Surface.h
        HEAP32[((surf+Runtime.QUANTUM_SIZE*4)>>2)]=width * bpp       // SDL_Surface.pitch, assuming RGBA or indexed for now,
                                                                                 // since that is what ImageData gives us in browsers
        HEAP32[((surf+Runtime.QUANTUM_SIZE*5)>>2)]=buffer      // SDL_Surface.pixels
        HEAP32[((surf+Runtime.QUANTUM_SIZE*6)>>2)]=0      // SDL_Surface.offset
        HEAP32[((pixelFormat + SDL.structs.PixelFormat.format)>>2)]=-2042224636 // SDL_PIXELFORMAT_RGBA8888
        HEAP32[((pixelFormat + SDL.structs.PixelFormat.palette)>>2)]=0 // TODO
        HEAP8[((pixelFormat + SDL.structs.PixelFormat.BitsPerPixel)|0)]=bpp * 8
        HEAP8[((pixelFormat + SDL.structs.PixelFormat.BytesPerPixel)|0)]=bpp
        HEAP32[((pixelFormat + SDL.structs.PixelFormat.Rmask)>>2)]=rmask || 0x000000ff
        HEAP32[((pixelFormat + SDL.structs.PixelFormat.Gmask)>>2)]=gmask || 0x0000ff00
        HEAP32[((pixelFormat + SDL.structs.PixelFormat.Bmask)>>2)]=bmask || 0x00ff0000
        HEAP32[((pixelFormat + SDL.structs.PixelFormat.Amask)>>2)]=amask || 0xff000000
        // Decide if we want to use WebGL or not
        var useWebGL = (flags & 0x04000000) != 0; // SDL_OPENGL
        SDL.GL = SDL.GL || useWebGL;
        var canvas;
        if (!usePageCanvas) {
          canvas = document.createElement('canvas');
          canvas.width = width;
          canvas.height = height;
        } else {
          canvas = Module['canvas'];
        }
        var ctx = Browser.createContext(canvas, useWebGL, usePageCanvas);
        SDL.surfaces[surf] = {
          width: width,
          height: height,
          canvas: canvas,
          ctx: ctx,
          surf: surf,
          buffer: buffer,
          pixelFormat: pixelFormat,
          alpha: 255,
          flags: flags,
          locked: 0,
          usePageCanvas: usePageCanvas,
          source: source,
          isFlagSet: function (flag) {
            return flags & flag;
          }
        };
        return surf;
      },copyIndexedColorData:function (surfData, rX, rY, rW, rH) {
        // HWPALETTE works with palette
        // setted by SDL_SetColors
        if (!surfData.colors) {
          return;
        }
        var fullWidth  = Module['canvas'].width;
        var fullHeight = Module['canvas'].height;
        var startX  = rX || 0;
        var startY  = rY || 0;
        var endX    = (rW || (fullWidth - startX)) + startX;
        var endY    = (rH || (fullHeight - startY)) + startY;
        var buffer  = surfData.buffer;
        var data    = surfData.image.data;
        var colors  = surfData.colors;
        for (var y = startY; y < endY; ++y) {
          var indexBase = y * fullWidth;
          var colorBase = indexBase * 4;
          for (var x = startX; x < endX; ++x) {
            // HWPALETTE have only 256 colors (not rgba)
            var index = HEAPU8[((buffer + indexBase + x)|0)] * 3;
            var colorOffset = colorBase + x * 4;
            data[colorOffset   ] = colors[index   ];
            data[colorOffset +1] = colors[index +1];
            data[colorOffset +2] = colors[index +2];
            //unused: data[colorOffset +3] = color[index +3];
          }
        }
      },freeSurface:function (surf) {
        _free(SDL.surfaces[surf].buffer);
        _free(SDL.surfaces[surf].pixelFormat);
        _free(surf);
        SDL.surfaces[surf] = null;
      },receiveEvent:function (event) {
        switch(event.type) {
          case 'mousemove':
            if (Browser.pointerLock) {
              // workaround for firefox bug 750111
              if ('mozMovementX' in event) {
                event['movementX'] = event['mozMovementX'];
                event['movementY'] = event['mozMovementY'];
              }
              // workaround for Firefox bug 782777
              if (event['movementX'] == 0 && event['movementY'] == 0) {
                // ignore a mousemove event if it doesn't contain any movement info
                // (without pointer lock, we infer movement from pageX/pageY, so this check is unnecessary)
                event.preventDefault();
                return;
              }
            }
            // fall through
          case 'keydown': case 'keyup': case 'keypress': case 'mousedown': case 'mouseup': case 'DOMMouseScroll': case 'mousewheel':
            if (event.type == 'DOMMouseScroll' || event.type == 'mousewheel') {
              var button = (event.type == 'DOMMouseScroll' ? event.detail : -event.wheelDelta) > 0 ? 4 : 3;
              var event2 = {
                type: 'mousedown',
                button: button,
                pageX: event.pageX,
                pageY: event.pageY
              };
              SDL.events.push(event2);
              event = {
                type: 'mouseup',
                button: button,
                pageX: event.pageX,
                pageY: event.pageY
              };
            } else if (event.type == 'mousedown') {
              SDL.DOMButtons[event.button] = 1;
            } else if (event.type == 'mouseup') {
              // ignore extra ups, can happen if we leave the canvas while pressing down, then return,
              // since we add a mouseup in that case
              if (!SDL.DOMButtons[event.button]) {
                event.preventDefault();
                return;
              }
              SDL.DOMButtons[event.button] = 0;
            }
            if (event.type == 'keypress' && !SDL.textInput) {
              break;
            }
            SDL.events.push(event);
            break;
          case 'mouseout':
            // Un-press all pressed mouse buttons, because we might miss the release outside of the canvas
            for (var i = 0; i < 3; i++) {
              if (SDL.DOMButtons[i]) {
                SDL.events.push({
                  type: 'mouseup',
                  button: i,
                  pageX: event.pageX,
                  pageY: event.pageY
                });
                SDL.DOMButtons[i] = 0;
              }
            }
            break;
          case 'blur':
          case 'visibilitychange': {
            // Un-press all pressed keys: TODO
            for (var code in SDL.keyboardMap) {
              SDL.events.push({
                type: 'keyup',
                keyCode: SDL.keyboardMap[code]
              });
            }
            break;
          }
          case 'unload':
            if (Browser.mainLoop.runner) {
              SDL.events.push(event);
              // Force-run a main event loop, since otherwise this event will never be caught!
              Browser.mainLoop.runner();
            }
            return;
          case 'resize':
            SDL.events.push(event);
            break;
        }
        if (SDL.events.length >= 10000) {
          Module.printErr('SDL event queue full, dropping events');
          SDL.events = SDL.events.slice(0, 10000);
        }
        // manually triggered resize event doesn't have a preventDefault member
        if (event.preventDefault) {
          event.preventDefault();
        }
        return;
      },makeCEvent:function (event, ptr) {
        if (typeof event === 'number') {
          // This is a pointer to a native C event that was SDL_PushEvent'ed
          _memcpy(ptr, event, SDL.structs.KeyboardEvent.__size__); // XXX
          return;
        }
        switch(event.type) {
          case 'keydown': case 'keyup': {
            var down = event.type === 'keydown';
            //Module.print('Received key event: ' + event.keyCode);
            var key = event.keyCode;
            if (key >= 65 && key <= 90) {
              key += 32; // make lowercase for SDL
            } else {
              key = SDL.keyCodes[event.keyCode] || event.keyCode;
            }
            var scan;
            if (key >= 1024) {
              scan = key - 1024;
            } else {
              scan = SDL.scanCodes[key] || key;
            }
            var code = SDL.keyCodes[event.keyCode] || event.keyCode;
            HEAP8[(((SDL.keyboardState)+(code))|0)]=down;
            if (down) {
              SDL.keyboardMap[code] = event.keyCode; // save the DOM input, which we can use to unpress it during blur
            } else {
              delete SDL.keyboardMap[code];
            }
            // TODO: lmeta, rmeta, numlock, capslock, KMOD_MODE, KMOD_RESERVED
            SDL.modState = (HEAP8[(((SDL.keyboardState)+(1248))|0)] ? 0x0040 | 0x0080 : 0) | // KMOD_LCTRL & KMOD_RCTRL
              (HEAP8[(((SDL.keyboardState)+(1249))|0)] ? 0x0001 | 0x0002 : 0) | // KMOD_LSHIFT & KMOD_RSHIFT
              (HEAP8[(((SDL.keyboardState)+(1250))|0)] ? 0x0100 | 0x0200 : 0); // KMOD_LALT & KMOD_RALT
            HEAP32[(((ptr)+(SDL.structs.KeyboardEvent.type))>>2)]=SDL.DOMEventToSDLEvent[event.type]
            HEAP8[(((ptr)+(SDL.structs.KeyboardEvent.state))|0)]=down ? 1 : 0
            HEAP8[(((ptr)+(SDL.structs.KeyboardEvent.repeat))|0)]=0 // TODO
            HEAP32[(((ptr)+(SDL.structs.KeyboardEvent.keysym + SDL.structs.keysym.scancode))>>2)]=scan
            HEAP32[(((ptr)+(SDL.structs.KeyboardEvent.keysym + SDL.structs.keysym.sym))>>2)]=key
            HEAP32[(((ptr)+(SDL.structs.KeyboardEvent.keysym + SDL.structs.keysym.mod))>>2)]=SDL.modState
            HEAP32[(((ptr)+(SDL.structs.KeyboardEvent.keysym + SDL.structs.keysym.unicode))>>2)]=key
            break;
          }
          case 'keypress': {
            HEAP32[(((ptr)+(SDL.structs.TextInputEvent.type))>>2)]=SDL.DOMEventToSDLEvent[event.type]
            // Not filling in windowID for now
            var cStr = intArrayFromString(String.fromCharCode(event.charCode));
            for (var i = 0; i < cStr.length; ++i) {
              HEAP8[(((ptr)+(SDL.structs.TextInputEvent.text + i))|0)]=cStr[i];
            }
            break;
          }
          case 'mousedown': case 'mouseup':
            if (event.type == 'mousedown') {
              // SDL_BUTTON(x) is defined as (1 << ((x)-1)).  SDL buttons are 1-3,
              // and DOM buttons are 0-2, so this means that the below formula is
              // correct.
              SDL.buttonState |= 1 << event.button;
            } else if (event.type == 'mouseup') {
              SDL.buttonState &= ~(1 << event.button);
            }
            // fall through
          case 'mousemove': {
            Browser.calculateMouseEvent(event);
            if (event.type != 'mousemove') {
              var down = event.type === 'mousedown';
              HEAP32[(((ptr)+(SDL.structs.MouseButtonEvent.type))>>2)]=SDL.DOMEventToSDLEvent[event.type];
              HEAP8[(((ptr)+(SDL.structs.MouseButtonEvent.button))|0)]=event.button+1; // DOM buttons are 0-2, SDL 1-3
              HEAP8[(((ptr)+(SDL.structs.MouseButtonEvent.state))|0)]=down ? 1 : 0;
              HEAP32[(((ptr)+(SDL.structs.MouseButtonEvent.x))>>2)]=Browser.mouseX;
              HEAP32[(((ptr)+(SDL.structs.MouseButtonEvent.y))>>2)]=Browser.mouseY;
            } else {
              HEAP32[(((ptr)+(SDL.structs.MouseMotionEvent.type))>>2)]=SDL.DOMEventToSDLEvent[event.type];
              HEAP8[(((ptr)+(SDL.structs.MouseMotionEvent.state))|0)]=SDL.buttonState;
              HEAP32[(((ptr)+(SDL.structs.MouseMotionEvent.x))>>2)]=Browser.mouseX;
              HEAP32[(((ptr)+(SDL.structs.MouseMotionEvent.y))>>2)]=Browser.mouseY;
              HEAP32[(((ptr)+(SDL.structs.MouseMotionEvent.xrel))>>2)]=Browser.mouseMovementX;
              HEAP32[(((ptr)+(SDL.structs.MouseMotionEvent.yrel))>>2)]=Browser.mouseMovementY;
            }
            break;
          }
          case 'unload': {
            HEAP32[(((ptr)+(SDL.structs.KeyboardEvent.type))>>2)]=SDL.DOMEventToSDLEvent[event.type];
            break;
          }
          case 'resize': {
            HEAP32[(((ptr)+(SDL.structs.KeyboardEvent.type))>>2)]=SDL.DOMEventToSDLEvent[event.type];
            HEAP32[(((ptr)+(SDL.structs.ResizeEvent.w))>>2)]=event.w;
            HEAP32[(((ptr)+(SDL.structs.ResizeEvent.h))>>2)]=event.h;
            break;
          }
          default: throw 'Unhandled SDL event: ' + event.type;
        }
      },estimateTextWidth:function (fontData, text) {
        var h = fontData.size;
        var fontString = h + 'px ' + fontData.name;
        var tempCtx = SDL.ttfContext;
        assert(tempCtx, 'TTF_Init must have been called');
        tempCtx.save();
        tempCtx.font = fontString;
        var ret = tempCtx.measureText(text).width | 0;
        tempCtx.restore();
        return ret;
      },allocateChannels:function (num) { // called from Mix_AllocateChannels and init
        if (SDL.numChannels && SDL.numChannels >= num) return;
        SDL.numChannels = num;
        SDL.channels = [];
        for (var i = 0; i < num; i++) {
          SDL.channels[i] = {
            audio: null,
            volume: 1.0
          };
        }
      },setGetVolume:function (info, volume) {
        if (!info) return 0;
        var ret = info.volume * 128; // MIX_MAX_VOLUME
        if (volume != -1) {
          info.volume = volume / 128;
          if (info.audio) info.audio.volume = info.volume;
        }
        return ret;
      },debugSurface:function (surfData) {
        console.log('dumping surface ' + [surfData.surf, surfData.source, surfData.width, surfData.height]);
        var image = surfData.ctx.getImageData(0, 0, surfData.width, surfData.height);
        var data = image.data;
        var num = Math.min(surfData.width, surfData.height);
        for (var i = 0; i < num; i++) {
          console.log('   diagonal ' + i + ':' + [data[i*surfData.width*4 + i*4 + 0], data[i*surfData.width*4 + i*4 + 1], data[i*surfData.width*4 + i*4 + 2], data[i*surfData.width*4 + i*4 + 3]]);
        }
      }};function _SDL_Init(what) {
      SDL.startTime = Date.now();
      // capture all key events. we just keep down and up, but also capture press to prevent default actions
      if (!Module['doNotCaptureKeyboard']) {
        document.addEventListener("keydown", SDL.receiveEvent);
        document.addEventListener("keyup", SDL.receiveEvent);
        document.addEventListener("keypress", SDL.receiveEvent);
        document.addEventListener("blur", SDL.receiveEvent);
        document.addEventListener("visibilitychange", SDL.receiveEvent);
      }
      window.addEventListener("unload", SDL.receiveEvent);
      SDL.keyboardState = _malloc(0x10000); // Our SDL needs 512, but 64K is safe for older SDLs
      _memset(SDL.keyboardState, 0, 0x10000);
      // Initialize this structure carefully for closure
      SDL.DOMEventToSDLEvent['keydown'] = 0x300 /* SDL_KEYDOWN */;
      SDL.DOMEventToSDLEvent['keyup'] = 0x301 /* SDL_KEYUP */;
      SDL.DOMEventToSDLEvent['keypress'] = 0x303 /* SDL_TEXTINPUT */;
      SDL.DOMEventToSDLEvent['mousedown'] = 0x401 /* SDL_MOUSEBUTTONDOWN */;
      SDL.DOMEventToSDLEvent['mouseup'] = 0x402 /* SDL_MOUSEBUTTONUP */;
      SDL.DOMEventToSDLEvent['mousemove'] = 0x400 /* SDL_MOUSEMOTION */;
      SDL.DOMEventToSDLEvent['unload'] = 0x100 /* SDL_QUIT */;
      SDL.DOMEventToSDLEvent['resize'] = 0x7001 /* SDL_VIDEORESIZE/SDL_EVENT_COMPAT2 */;
      return 0; // success
    }
  function _SDL_UpperBlit(src, srcrect, dst, dstrect) {
      var srcData = SDL.surfaces[src];
      var dstData = SDL.surfaces[dst];
      var sr, dr;
      if (srcrect) {
        sr = SDL.loadRect(srcrect);
      } else {
        sr = { x: 0, y: 0, w: srcData.width, h: srcData.height };
      }
      if (dstrect) {
        dr = SDL.loadRect(dstrect);
      } else {
        dr = { x: 0, y: 0, w: -1, h: -1 };
      }
      dstData.ctx.drawImage(srcData.canvas, sr.x, sr.y, sr.w, sr.h, dr.x, dr.y, sr.w, sr.h);
      if (dst != SDL.screen) {
        // XXX As in IMG_Load, for compatibility we write out |pixels|
        console.log('WARNING: copying canvas data to memory for compatibility');
        _SDL_LockSurface(dst);
        dstData.locked--; // The surface is not actually locked in this hack
      }
      return 0;
    }
  function _SDL_SetVideoMode(width, height, depth, flags) {
      ['mousedown', 'mouseup', 'mousemove', 'DOMMouseScroll', 'mousewheel', 'mouseout'].forEach(function(event) {
        Module['canvas'].addEventListener(event, SDL.receiveEvent, true);
      });
      Browser.setCanvasSize(width, height, true);
      // Free the old surface first.
      if (SDL.screen) {
        SDL.freeSurface(SDL.screen);
        SDL.screen = null;
      }
      SDL.screen = SDL.makeSurface(width, height, flags, true, 'screen');
      if (!SDL.addedResizeListener) {
        SDL.addedResizeListener = true;
        Browser.resizeListeners.push(function(w, h) {
          SDL.receiveEvent({
            type: 'resize',
            w: w,
            h: h
          });
        });
      }
      return SDL.screen;
    }
  function _SDL_UpdateRect(surf, x, y, w, h) {
      // We actually do the whole screen in Unlock...
    }
  function ___cxa_pure_virtual() {
      ABORT = true;
      throw 'Pure virtual function called!';
    }
  function _SDL_GetTicks() {
      return Math.floor(Date.now() - SDL.startTime);
    }
  function _memset(ptr, value, num) {
      ptr = ptr|0; value = value|0; num = num|0;
      var stop = 0, value4 = 0, stop4 = 0, unaligned = 0;
      stop = (ptr + num)|0;
      if ((num|0) >= 20) {
        // This is unaligned, but quite large, so work hard to get to aligned settings
        value = value & 0xff;
        unaligned = ptr & 3;
        value4 = value | (value << 8) | (value << 16) | (value << 24);
        stop4 = stop & ~3;
        if (unaligned) {
          unaligned = (ptr + 4 - unaligned)|0;
          while ((ptr|0) < (unaligned|0)) { // no need to check for stop, since we have large num
            HEAP8[(ptr)]=value;
            ptr = (ptr+1)|0;
          }
        }
        while ((ptr|0) < (stop4|0)) {
          HEAP32[((ptr)>>2)]=value4;
          ptr = (ptr+4)|0;
        }
      }
      while ((ptr|0) < (stop|0)) {
        HEAP8[(ptr)]=value;
        ptr = (ptr+1)|0;
      }
    }var _llvm_memset_p0i8_i64=_memset;
  var _llvm_expect_i32=undefined;
  function _abort() {
      ABORT = true;
      throw 'abort() at ' + (new Error().stack);
    }
  function ___errno_location() {
      return ___errno_state;
    }var ___errno=___errno_location;
  function _memcpy(dest, src, num) {
      dest = dest|0; src = src|0; num = num|0;
      var ret = 0;
      ret = dest|0;
      if ((dest&3) == (src&3)) {
        while (dest & 3) {
          if ((num|0) == 0) return ret|0;
          HEAP8[(dest)]=HEAP8[(src)];
          dest = (dest+1)|0;
          src = (src+1)|0;
          num = (num-1)|0;
        }
        while ((num|0) >= 4) {
          HEAP32[((dest)>>2)]=HEAP32[((src)>>2)];
          dest = (dest+4)|0;
          src = (src+4)|0;
          num = (num-4)|0;
        }
      }
      while ((num|0) > 0) {
        HEAP8[(dest)]=HEAP8[(src)];
        dest = (dest+1)|0;
        src = (src+1)|0;
        num = (num-1)|0;
      }
      return ret|0;
    }var _llvm_memcpy_p0i8_p0i8_i32=_memcpy;
  function _sysconf(name) {
      // long sysconf(int name);
      // http://pubs.opengroup.org/onlinepubs/009695399/functions/sysconf.html
      switch(name) {
        case 8: return PAGE_SIZE;
        case 54:
        case 56:
        case 21:
        case 61:
        case 63:
        case 22:
        case 67:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 69:
        case 28:
        case 101:
        case 70:
        case 71:
        case 29:
        case 30:
        case 199:
        case 75:
        case 76:
        case 32:
        case 43:
        case 44:
        case 80:
        case 46:
        case 47:
        case 45:
        case 48:
        case 49:
        case 42:
        case 82:
        case 33:
        case 7:
        case 108:
        case 109:
        case 107:
        case 112:
        case 119:
        case 121:
          return 200809;
        case 13:
        case 104:
        case 94:
        case 95:
        case 34:
        case 35:
        case 77:
        case 81:
        case 83:
        case 84:
        case 85:
        case 86:
        case 87:
        case 88:
        case 89:
        case 90:
        case 91:
        case 94:
        case 95:
        case 110:
        case 111:
        case 113:
        case 114:
        case 115:
        case 116:
        case 117:
        case 118:
        case 120:
        case 40:
        case 16:
        case 79:
        case 19:
          return -1;
        case 92:
        case 93:
        case 5:
        case 72:
        case 6:
        case 74:
        case 92:
        case 93:
        case 96:
        case 97:
        case 98:
        case 99:
        case 102:
        case 103:
        case 105:
          return 1;
        case 38:
        case 66:
        case 50:
        case 51:
        case 4:
          return 1024;
        case 15:
        case 64:
        case 41:
          return 32;
        case 55:
        case 37:
        case 17:
          return 2147483647;
        case 18:
        case 1:
          return 47839;
        case 59:
        case 57:
          return 99;
        case 68:
        case 58:
          return 2048;
        case 0: return 2097152;
        case 3: return 65536;
        case 14: return 32768;
        case 73: return 32767;
        case 39: return 16384;
        case 60: return 1000;
        case 106: return 700;
        case 52: return 256;
        case 62: return 255;
        case 2: return 100;
        case 65: return 64;
        case 36: return 20;
        case 100: return 16;
        case 20: return 6;
        case 53: return 4;
        case 10: return 1;
      }
      ___setErrNo(ERRNO_CODES.EINVAL);
      return -1;
    }
  function _time(ptr) {
      var ret = Math.floor(Date.now()/1000);
      if (ptr) {
        HEAP32[((ptr)>>2)]=ret
      }
      return ret;
    }
  function _sbrk(bytes) {
      // Implement a Linux-like 'memory area' for our 'process'.
      // Changes the size of the memory area by |bytes|; returns the
      // address of the previous top ('break') of the memory area
      // We control the "dynamic" memory - DYNAMIC_BASE to DYNAMICTOP
      var self = _sbrk;
      if (!self.called) {
        DYNAMICTOP = alignMemoryPage(DYNAMICTOP); // make sure we start out aligned
        self.called = true;
        assert(Runtime.dynamicAlloc);
        self.alloc = Runtime.dynamicAlloc;
        Runtime.dynamicAlloc = function() { abort('cannot dynamically allocate, sbrk now has control') };
      }
      var ret = DYNAMICTOP;
      if (bytes != 0) self.alloc(bytes);
      return ret;  // Previous break location.
    }
__ATINIT__.unshift({ func: function() { if (!Module["noFSInit"] && !FS.init.initialized) FS.init() } });__ATMAIN__.push({ func: function() { FS.ignorePermissions = false } });__ATEXIT__.push({ func: function() { FS.quit() } });Module["FS_createFolder"] = FS.createFolder;Module["FS_createPath"] = FS.createPath;Module["FS_createDataFile"] = FS.createDataFile;Module["FS_createPreloadedFile"] = FS.createPreloadedFile;Module["FS_createLazyFile"] = FS.createLazyFile;Module["FS_createLink"] = FS.createLink;Module["FS_createDevice"] = FS.createDevice;
___errno_state = Runtime.staticAlloc(4); HEAP32[((___errno_state)>>2)]=0;
Module["requestFullScreen"] = function(lockPointer, resizeCanvas) { Browser.requestFullScreen(lockPointer, resizeCanvas) };
  Module["requestAnimationFrame"] = function(func) { Browser.requestAnimationFrame(func) };
  Module["pauseMainLoop"] = function() { Browser.mainLoop.pause() };
  Module["resumeMainLoop"] = function() { Browser.mainLoop.resume() };
  Module["getUserMedia"] = function() { Browser.getUserMedia() }
STACK_BASE = STACKTOP = Runtime.alignMemory(STATICTOP);
staticSealed = true; // seal the static portion of memory
STACK_MAX = STACK_BASE + 5242880;
DYNAMIC_BASE = DYNAMICTOP = Runtime.alignMemory(STACK_MAX);
assert(DYNAMIC_BASE < TOTAL_MEMORY); // Stack must fit in TOTAL_MEMORY; allocations from here on may enlarge TOTAL_MEMORY
var FUNCTION_TABLE = [0,0,__ZN10__cxxabiv116__shim_type_infoD2Ev,0,__ZN14ddSurfaceClass13getRealHeightEv,0,__ZN16timeWrapperClassD0Ev,0,__ZN14ddSurfaceClass8getWidthEv,0,__ZNK10__cxxabiv117__class_type_info16search_above_dstEPNS_19__dynamic_cast_infoEPKvS4_ib
,0,__ZN10__cxxabiv116__shim_type_infoD2Ev,0,__ZN15directDrawClassD0Ev,0,___cxa_pure_virtual,0,__ZN10__cxxabiv116__shim_type_infoD2Ev,0,__ZN14ddSurfaceClass17getDwordFromPixelEiiRm
,0,__ZN10__cxxabiv117__class_type_infoD0Ev,0,__ZN14ddSurfaceClass7bltFromERS_iiiiiiiiiibb,0,__ZN15directDrawClassD1Ev,0,__ZN14ddSurfaceClass6isLostEPb,0,__ZN14ddSurfaceClass9writeTextEPciii9alignEnumiiii
,0,__ZN16loopWrapperClass14getTimerObjectEv,0,__ZN14ddSurfaceClass8drawLineEiiiiiii,0,__ZN16timeWrapperClassD1Ev,0,__ZN14ddSurfaceClass14stretchBltFromERS_iiiiiiiibbb,0,__ZNK10__cxxabiv117__class_type_info27has_unambiguous_public_baseEPNS_19__dynamic_cast_infoEPvi
,0,__ZN16loopWrapperClass27getFramesToExecutePerSecondEv,0,__ZN16loopWrapperClass14setActiveStateEb,0,__ZN14ddSurfaceClass7restoreEv,0,__ZN13mainLoopClass9afterLoopEv,0,__ZN13mainLoopClass10beforeLoopEv
,0,__ZN14ddSurfaceClass13calculateTextEPc9alignEnumRiS2_,0,__ZN16loopWrapperClass15executeMainLoopEiim,0,__ZN14ddSurfaceClass7bltFromERS_iiiiiibb,0,__ZN10__cxxabiv120__si_class_type_infoD0Ev,0,__ZN10__cxxabiv116__shim_type_infoD2Ev
,0,__ZN16loopWrapperClassD0Ev,0,__ZNK10__cxxabiv117__class_type_info9can_catchEPKNS_16__shim_type_infoERPv,0,__ZNK10__cxxabiv120__si_class_type_info16search_above_dstEPNS_19__dynamic_cast_infoEPKvS4_ib,0,__ZNK10__cxxabiv117__class_type_info16search_below_dstEPNS_19__dynamic_cast_infoEPKvib,0,__ZN14ddSurfaceClass6unlockEv
,0,__ZN16loopWrapperClass3endEv,0,__ZN15directDrawClass7isReadyEv,0,__ZN14ddSurfaceClass9getHeightEv,0,__ZN14ddSurfaceClassD0Ev,0,__ZN14ddSurfaceClass7bltFromERS_iibb
,0,__ZN14ddSurfaceClass12getRealWidthEv,0,__ZNK10__cxxabiv116__shim_type_info5noop1Ev,0,__ZN14ddSurfaceClass12alphaBltFromERS_iiib,0,__ZN14ddSurfaceClass11getDDObjectEv,0,__ZN15directDrawClass7restoreEv
,0,__ZN14ddSurfaceClass12fillAllDwordEm,0,__ZN14ddSurfaceClass15isInVideoMemoryEv,0,__ZN14ddSurfaceClass13saveAsBmpFileEPcb,0,__ZN13mainLoopClassD1Ev,0,__ZN13mainLoopClassD0Ev
,0,__ZN10__cxxabiv116__shim_type_infoD2Ev,0,__ZN15directDrawClass6isLostEPb,0,__ZN13mainLoopClass12executeFrameEm,0,__ZN14ddSurfaceClass15obscuredBltFromERS_iiib,0,__ZN14ddSurfaceClass15getDwordFromRGBEiiiRm
,0,__ZN14ddSurfaceClass4lockEbbPmPPv,0,__ZN13mainLoopClass9drawFrameEv,0,__ZN16loopWrapperClassD1Ev,0,__ZNK10__cxxabiv116__shim_type_info5noop2Ev,0,__ZN14ddSurfaceClass8putPixelEiiiii
,0,__ZNK10__cxxabiv120__si_class_type_info16search_below_dstEPNS_19__dynamic_cast_infoEPKvib,0,__ZN16loopWrapperClass8exitLoopEv,0,__ZN16loopWrapperClass12isLoopActiveEv,0,__ZN13mainLoopClass21informFramesPerSecondEd,0,__ZN14ddSurfaceClass8fillRectEiiiiiii,0,__ZN14ddSurfaceClassD1Ev,0,__ZNK10__cxxabiv120__si_class_type_info27has_unambiguous_public_baseEPNS_19__dynamic_cast_infoEPvi];
// EMSCRIPTEN_START_FUNCS
function __Z18checkLostResourcesv() {
  var label = 0;
  return 1; //@line 336 "globals.cpp"
}
function __ZN14conLoggerClass4initEPcP12consoleClassii($this, $name, $console, $bytesBufferName, $bytesStrBuffer) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  $1=$this;
  $2=$name;
  $3=$console;
  $4=$bytesBufferName;
  $5=$bytesStrBuffer;
  var $6=$1;
  return 1; //@line 60 "con-logger.cpp"
}
function __ZN13mainLoopClass10beforeLoopEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  return 1; //@line 52 "mainloop.cpp"
}
function __ZN13mainLoopClass12executeFrameEm($this, $frameNumber) {
  var label = 0;
  var $1;
  var $2;
  $1=$this;
  $2=$frameNumber;
  var $3=$1;
  return 1; //@line 215 "mainloop.cpp"
}
function __Z8closeAllPc($errorText) {
  var label = 0;
  var $1;
  $1=$errorText;
  return 1; //@line 386 "globals.cpp"
}
function __ZN14ddSurfaceClassD2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  return; //@line 128 "./ddwrap.h"
}
function __Z19switchToProgramZone16programZonesEnummmb($programZone, $programSubZone, $programSubSubZone, $reInitializeAllResources) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  $1=$programZone;
  $2=$programSubZone;
  $3=$programSubSubZone;
  var $5=(($reInitializeAllResources)&(1));
  $4=$5;
  return 1; //@line 106 "programzones.cpp"
}
function __ZN16loopWrapperClass8exitLoopEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=(($2+36)|0); //@line 67 "./loopwrap.h"
  HEAP8[($3)]=1; //@line 67 "./loopwrap.h"
  return; //@line 67 "./loopwrap.h"
}
function __ZN16loopWrapperClass12isLoopActiveEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=(($2+37)|0); //@line 68 "./loopwrap.h"
  var $4=HEAP8[($3)]; //@line 68 "./loopwrap.h"
  var $5=(($4) & 1); //@line 68 "./loopwrap.h"
  return $5; //@line 68 "./loopwrap.h"
}
function __ZN16loopWrapperClass14setActiveStateEb($this, $active) {
  var label = 0;
  var $1;
  var $2;
  $1=$this;
  var $3=(($active)&(1));
  $2=$3;
  var $4=$1;
  var $5=$2; //@line 69 "./loopwrap.h"
  var $6=(($5) & 1); //@line 69 "./loopwrap.h"
  var $7=(($4+37)|0); //@line 69 "./loopwrap.h"
  var $8=(($6)&(1)); //@line 69 "./loopwrap.h"
  HEAP8[($7)]=$8; //@line 69 "./loopwrap.h"
  return; //@line 69 "./loopwrap.h"
}
function __ZN16loopWrapperClass14getTimerObjectEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=(($2+8)|0); //@line 70 "./loopwrap.h"
  return $3; //@line 70 "./loopwrap.h"
}
function __ZN16loopWrapperClass27getFramesToExecutePerSecondEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=(($2+32)|0); //@line 71 "./loopwrap.h"
  var $4=HEAP32[(($3)>>2)]; //@line 71 "./loopwrap.h"
  return $4; //@line 71 "./loopwrap.h"
}
function __ZN16timeWrapperClassC2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=$2; //@line 67 "./timewrap.h"
  HEAP32[(($3)>>2)]=((888)|0); //@line 67 "./timewrap.h"
  var $4=(($2+4)|0); //@line 67 "./timewrap.h"
  HEAP8[($4)]=0; //@line 67 "./timewrap.h"
  return; //@line 67 "./timewrap.h"
}
function __ZN14ddSurfaceClassC2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=$2; //@line 126 "./ddwrap.h"
  HEAP32[(($3)>>2)]=((1112)|0); //@line 126 "./ddwrap.h"
  var $4=(($2+21)|0); //@line 126 "./ddwrap.h"
  HEAP8[($4)]=0; //@line 126 "./ddwrap.h"
  var $5=(($2+28)|0); //@line 126 "./ddwrap.h"
  HEAP8[($5)]=0; //@line 126 "./ddwrap.h"
  return; //@line 126 "./ddwrap.h"
}
function __ZN14conLoggerClass7logLineEPcbbbz($this, $text, $file, $console, $noUnselError, varrp) {
  var label = 0;
  var __stackBase__  = STACKTOP; STACKTOP = (STACKTOP + 16)|0; assert((STACKTOP|0) < (STACK_MAX|0));
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  var $paramList=__stackBase__;
  $1=$this;
  $2=$text;
  var $6=(($file)&(1));
  $3=$6;
  var $7=(($console)&(1));
  $4=$7;
  var $8=(($noUnselError)&(1));
  $5=$8;
  var $9=$1;
  var $10=(($paramList)|0); //@line 169 "con-logger.cpp"
  var $11=$10; //@line 169 "con-logger.cpp"
  HEAP32[(($11)>>2)]=varrp;HEAP32[((($11)+(4))>>2)]=0; //@line 169 "con-logger.cpp"
  var $12=$2; //@line 170 "con-logger.cpp"
  var $13=(($paramList)|0); //@line 170 "con-logger.cpp"
  var $14=_vprintf($12, $13); //@line 170 "con-logger.cpp"
  var $15=_printf(((400)|0), (tempInt=STACKTOP,STACKTOP = (STACKTOP + 1)|0,STACKTOP = ((((STACKTOP)+7)>>3)<<3),assert((STACKTOP|0) < (STACK_MAX|0)),HEAP32[((tempInt)>>2)]=0,tempInt)); //@line 171 "con-logger.cpp"
  var $16=(($paramList)|0); //@line 172 "con-logger.cpp"
  var $17=$16; //@line 172 "con-logger.cpp"
 //@line 172 "con-logger.cpp"
  STACKTOP = __stackBase__;
  return 1; //@line 214 "con-logger.cpp"
}
function __Z23switchToNullProgramZonev() {
  var label = 0;
  var $1=__Z19switchToProgramZone16programZonesEnummmb(0, 0, 0, 1); //@line 115 "programzones.cpp"
  return $1; //@line 115 "programzones.cpp"
}
function __ZN13mainLoopClass21informFramesPerSecondEd($this, $fps) {
  var label = 0;
  var __stackBase__  = STACKTOP; assert((STACKTOP|0) < (STACK_MAX|0));
  var $1;
  var $2;
  $1=$this;
  $2=$fps;
  var $3=$1;
  var $4=$2; //@line 44 "mainloop.cpp"
  var $5=__ZN14conLoggerClass7logLineEPcbbbz(_logger, ((376)|0), 1, 1, 1, (tempInt=STACKTOP,STACKTOP = (STACKTOP + 8)|0,assert((STACKTOP|0) < (STACK_MAX|0)),HEAPF64[((tempInt)>>3)]=$4,tempInt)); //@line 44 "mainloop.cpp"
  STACKTOP = __stackBase__;
  return 1; //@line 46 "mainloop.cpp"
}
function __ZN13mainLoopClass9afterLoopEv($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      $2=$this;
      var $3=$2;
      var $4=__Z23switchToNullProgramZonev(); //@line 59 "mainloop.cpp"
      if ($4) { label = 3; break; } else { label = 2; break; } //@line 59 "mainloop.cpp"
    case 2: 
      $1=0; //@line 60 "mainloop.cpp"
      label = 4; break; //@line 60 "mainloop.cpp"
    case 3: 
      $1=1; //@line 62 "mainloop.cpp"
      label = 4; break; //@line 62 "mainloop.cpp"
    case 4: 
      var $8=$1; //@line 63 "mainloop.cpp"
      return $8; //@line 63 "mainloop.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN13mainLoopClass9drawFrameEv($this) {
  var label = 0;
  var __stackBase__  = STACKTOP; assert((STACKTOP|0) < (STACK_MAX|0));
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $retval;
      $2=$this;
      var $3=$2;
      $retval=0; //@line 76 "mainloop.cpp"
      var $4=__Z18checkLostResourcesv(); //@line 81 "mainloop.cpp"
      if ($4) { label = 3; break; } else { label = 2; break; } //@line 81 "mainloop.cpp"
    case 2: 
      var $6=__ZN14conLoggerClass7logLineEPcbbbz(_logger, ((328)|0), 1, 1, 1, (tempInt=STACKTOP,STACKTOP = (STACKTOP + 1)|0,STACKTOP = ((((STACKTOP)+7)>>3)<<3),assert((STACKTOP|0) < (STACK_MAX|0)),HEAP32[((tempInt)>>2)]=0,tempInt)); //@line 82 "mainloop.cpp"
      $1=0; //@line 83 "mainloop.cpp"
      label = 4; break; //@line 83 "mainloop.cpp"
    case 3: 
      var $8=__ZN15directDrawClass4flipEv(1704); //@line 133 "mainloop.cpp"
      var $9=$retval; //@line 135 "mainloop.cpp"
      var $10=(($9) & 1); //@line 135 "mainloop.cpp"
      $1=$10; //@line 135 "mainloop.cpp"
      label = 4; break; //@line 135 "mainloop.cpp"
    case 4: 
      var $12=$1; //@line 136 "mainloop.cpp"
      STACKTOP = __stackBase__;
      return $12; //@line 136 "mainloop.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN13mainLoopClassD1Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN13mainLoopClassD2Ev($2); //@line 52 "./mainloop.h"
  return; //@line 52 "./mainloop.h"
}
function __ZN13mainLoopClassD0Ev($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      $1=$this;
      var $4=$1;
      (function() { try { __THREW__ = 0; return __ZN13mainLoopClassD1Ev($4) } catch(e) { if (typeof e != "number") throw e; if (ABORT) throw e; __THREW__ = 1; return null } })();if (!__THREW__) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $6=$4; //@line 52 "./mainloop.h"
      __ZdlPv($6); //@line 52 "./mainloop.h"
      return; //@line 52 "./mainloop.h"
    case 3: 
      var $8$0 = ___cxa_find_matching_catch(-1, -1); $8$1 = tempRet0;
      var $9=$8$0;
      $2=$9; //@line 52 "./mainloop.h"
      var $10=$8$1;
      $3=$10; //@line 52 "./mainloop.h"
      var $11=$4; //@line 52 "./mainloop.h"
      __ZdlPv($11); //@line 52 "./mainloop.h"
      label = 4; break; //@line 52 "./mainloop.h"
    case 4: 
      var $13=$2; //@line 52 "./mainloop.h"
      var $14=$3; //@line 52 "./mainloop.h"
      var $15$0=$13;
      var $15$1=0;
      var $16$0=$15$0;
      var $16$1=$14;
      ___resumeException($16$0) //@line 52 "./mainloop.h"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN16loopWrapperClass3endEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=$2; //@line 64 "./loopwrap.h"
  var $4=HEAP32[(($3)>>2)]; //@line 64 "./loopwrap.h"
  var $5=(($4+16)|0); //@line 64 "./loopwrap.h"
  var $6=HEAP32[(($5)>>2)]; //@line 64 "./loopwrap.h"
  FUNCTION_TABLE[$6]($2); //@line 64 "./loopwrap.h"
  return 1; //@line 64 "./loopwrap.h"
}
function __ZN13mainLoopClassD2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=$2; //@line 52 "./mainloop.h"
  __ZN16loopWrapperClassD2Ev($3); //@line 52 "./mainloop.h"
  return; //@line 52 "./mainloop.h"
}
function __ZN16timeWrapperClassD1Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN16timeWrapperClassD2Ev($2); //@line 68 "./timewrap.h"
  return; //@line 68 "./timewrap.h"
}
function __ZN16timeWrapperClassD2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=$2; //@line 68 "./timewrap.h"
  HEAP32[(($3)>>2)]=((888)|0); //@line 68 "./timewrap.h"
  var $4=__ZN16timeWrapperClass3endEv($2); //@line 68 "./timewrap.h"
  return; //@line 68 "./timewrap.h"
}
function __ZN16timeWrapperClassD0Ev($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      $1=$this;
      var $4=$1;
      (function() { try { __THREW__ = 0; return __ZN16timeWrapperClassD1Ev($4) } catch(e) { if (typeof e != "number") throw e; if (ABORT) throw e; __THREW__ = 1; return null } })();if (!__THREW__) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $6=$4; //@line 68 "./timewrap.h"
      __ZdlPv($6); //@line 68 "./timewrap.h"
      return; //@line 68 "./timewrap.h"
    case 3: 
      var $8$0 = ___cxa_find_matching_catch(-1, -1); $8$1 = tempRet0;
      var $9=$8$0;
      $2=$9; //@line 68 "./timewrap.h"
      var $10=$8$1;
      $3=$10; //@line 68 "./timewrap.h"
      var $11=$4; //@line 68 "./timewrap.h"
      __ZdlPv($11); //@line 68 "./timewrap.h"
      label = 4; break; //@line 68 "./timewrap.h"
    case 4: 
      var $13=$2; //@line 68 "./timewrap.h"
      var $14=$3; //@line 68 "./timewrap.h"
      var $15$0=$13;
      var $15$1=0;
      var $16$0=$15$0;
      var $16$1=$14;
      ___resumeException($16$0) //@line 68 "./timewrap.h"
    default: assert(0, "bad label: " + label);
  }
}
function ___cxx_global_var_init() {
  var label = 0;
  __ZN15directDrawClassC1Ev(1704); //@line 56 "globals.cpp"
  var $1=_atexit((26), 1704, ___dso_handle); //@line 56 "globals.cpp"
  return; //@line 56 "globals.cpp"
}
function __ZN15directDrawClassC1Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN15directDrawClassC2Ev($2); //@line 222 "./ddwrap.h"
  return; //@line 222 "./ddwrap.h"
}
function __ZN15directDrawClassD1Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN15directDrawClassD2Ev($2); //@line 223 "./ddwrap.h"
  return; //@line 223 "./ddwrap.h"
}
function ___cxx_global_var_init1() {
  var label = 0;
  __ZN13mainLoopClassC1Ev(88); //@line 112 "globals.cpp"
  var $1=_atexit((98), 88, ___dso_handle); //@line 112 "globals.cpp"
  return; //@line 112 "globals.cpp"
}
function __ZN13mainLoopClassC1Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN13mainLoopClassC2Ev($2); //@line 51 "./mainloop.h"
  return; //@line 51 "./mainloop.h"
}
function __Z15initializeFirstv() {
  var label = 0;
  var __stackBase__  = STACKTOP; assert((STACKTOP|0) < (STACK_MAX|0));
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2=__ZN14conLoggerClass4initEPcP12consoleClassii(_logger, ((200)|0), 0, 100, 200); //@line 130 "globals.cpp"
      var $3=__ZN14conLoggerClass7logLineEPcbbbz(_logger, ((280)|0), 1, 1, 1, (tempInt=STACKTOP,STACKTOP = (STACKTOP + 1)|0,STACKTOP = ((((STACKTOP)+7)>>3)<<3),assert((STACKTOP|0) < (STACK_MAX|0)),HEAP32[((tempInt)>>2)]=0,tempInt)); //@line 131 "globals.cpp"
      var $4=__ZN14conLoggerClass7logLineEPcbbbz(_logger, ((248)|0), 1, 1, 1, (tempInt=STACKTOP,STACKTOP = (STACKTOP + 1)|0,STACKTOP = ((((STACKTOP)+7)>>3)<<3),assert((STACKTOP|0) < (STACK_MAX|0)),HEAP32[((tempInt)>>2)]=0,tempInt)); //@line 132 "globals.cpp"
      var $5=__ZN14conLoggerClass7logLineEPcbbbz(_logger, ((216)|0), 1, 0, 1, (tempInt=STACKTOP,STACKTOP = (STACKTOP + 1)|0,STACKTOP = ((((STACKTOP)+7)>>3)<<3),assert((STACKTOP|0) < (STACK_MAX|0)),HEAP32[((tempInt)>>2)]=0,tempInt)); //@line 134 "globals.cpp"
      var $6=__ZN14conLoggerClass7logLineEPcbbbz(_logger, ((176)|0), 1, 1, 1, (tempInt=STACKTOP,STACKTOP = (STACKTOP + 8)|0,assert((STACKTOP|0) < (STACK_MAX|0)),HEAP32[((tempInt)>>2)]=((8)|0),tempInt)); //@line 138 "globals.cpp"
      var $7=_SDL_Init(32); //@line 174 "globals.cpp"
      var $8=__ZN15directDrawClass4initEv(1704); //@line 177 "globals.cpp"
      if ($8) { label = 3; break; } else { label = 2; break; } //@line 177 "globals.cpp"
    case 2: 
      var $10=__Z8closeAllPc(((160)|0)); //@line 179 "globals.cpp"
      $1=0; //@line 180 "globals.cpp"
      label = 6; break; //@line 180 "globals.cpp"
    case 3: 
      var $12=__ZN15directDrawClass12setVideoModeEiiii(1704, 800, 600, 16, 1); //@line 188 "globals.cpp"
      if ($12) { label = 5; break; } else { label = 4; break; } //@line 188 "globals.cpp"
    case 4: 
      var $14=__Z8closeAllPc(((136)|0)); //@line 193 "globals.cpp"
      $1=0; //@line 194 "globals.cpp"
      label = 6; break; //@line 194 "globals.cpp"
    case 5: 
      var $16=HEAP32[((1704)>>2)]; //@line 197 "globals.cpp"
      var $17=(($16+44)|0); //@line 197 "globals.cpp"
      var $18=HEAP32[(($17)>>2)]; //@line 197 "globals.cpp"
      var $19=FUNCTION_TABLE[$18](((1704)|0), 0); //@line 197 "globals.cpp"
      var $20=__ZN15directDrawClass4flipEv(1704); //@line 198 "globals.cpp"
      $1=1; //@line 300 "globals.cpp"
      label = 6; break; //@line 300 "globals.cpp"
    case 6: 
      var $22=$1; //@line 301 "globals.cpp"
      STACKTOP = __stackBase__;
      return $22; //@line 301 "globals.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN13mainLoopClassC2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=$2; //@line 51 "./mainloop.h"
  __ZN16loopWrapperClassC2Ev($3); //@line 51 "./mainloop.h"
  var $4=$2; //@line 51 "./mainloop.h"
  HEAP32[(($4)>>2)]=((1240)|0); //@line 51 "./mainloop.h"
  return; //@line 51 "./mainloop.h"
}
function __ZN16loopWrapperClassC2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=$2; //@line 59 "./loopwrap.h"
  HEAP32[(($3)>>2)]=((912)|0); //@line 59 "./loopwrap.h"
  var $4=(($2+8)|0); //@line 59 "./loopwrap.h"
  __ZN16timeWrapperClassC1Ev($4); //@line 59 "./loopwrap.h"
  return; //@line 59 "./loopwrap.h"
}
function __ZN16timeWrapperClassC1Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN16timeWrapperClassC2Ev($2); //@line 67 "./timewrap.h"
  return; //@line 67 "./timewrap.h"
}
function __ZN15directDrawClassC2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=$2; //@line 220 "./ddwrap.h"
  __ZN14ddSurfaceClassC2Ev($3); //@line 220 "./ddwrap.h"
  var $4=$2; //@line 220 "./ddwrap.h"
  HEAP32[(($4)>>2)]=((984)|0); //@line 220 "./ddwrap.h"
  var $5=(($2+36)|0); //@line 220 "./ddwrap.h"
  HEAP8[($5)]=0; //@line 220 "./ddwrap.h"
  var $6=(($2+37)|0); //@line 220 "./ddwrap.h"
  HEAP8[($6)]=0; //@line 220 "./ddwrap.h"
  var $7=$2; //@line 221 "./ddwrap.h"
  var $8=(($7+20)|0); //@line 221 "./ddwrap.h"
  HEAP8[($8)]=1; //@line 221 "./ddwrap.h"
  return; //@line 222 "./ddwrap.h"
}
function __GLOBAL__I_a() {
  var label = 0;
  ___cxx_global_var_init(); //@line 126 "globals.cpp"
  ___cxx_global_var_init1(); //@line 126 "globals.cpp"
  return; //@line 126 "globals.cpp"
}
function __ZN14ddSurfaceClass7bltFromERS_iibb($this, $srf, $xD, $yD, $clip, $transparent) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  var $6;
  $1=$this;
  $2=$srf;
  $3=$xD;
  $4=$yD;
  var $7=(($clip)&(1));
  $5=$7;
  var $8=(($transparent)&(1));
  $6=$8;
  var $9=$1;
  var $10=$9; //@line 47 "ddwrap.cpp"
  var $11=HEAP32[(($10)>>2)]; //@line 47 "ddwrap.cpp"
  var $12=(($11+12)|0); //@line 47 "ddwrap.cpp"
  var $13=HEAP32[(($12)>>2)]; //@line 47 "ddwrap.cpp"
  var $14=$2; //@line 47 "ddwrap.cpp"
  var $15=$3; //@line 47 "ddwrap.cpp"
  var $16=$4; //@line 47 "ddwrap.cpp"
  var $17=$2; //@line 47 "ddwrap.cpp"
  var $18=(($17+4)|0); //@line 47 "ddwrap.cpp"
  var $19=HEAP32[(($18)>>2)]; //@line 47 "ddwrap.cpp"
  var $20=$2; //@line 47 "ddwrap.cpp"
  var $21=(($20+8)|0); //@line 47 "ddwrap.cpp"
  var $22=HEAP32[(($21)>>2)]; //@line 47 "ddwrap.cpp"
  var $23=$5; //@line 47 "ddwrap.cpp"
  var $24=(($23) & 1); //@line 47 "ddwrap.cpp"
  var $25=$6; //@line 47 "ddwrap.cpp"
  var $26=(($25) & 1); //@line 47 "ddwrap.cpp"
  var $27=FUNCTION_TABLE[$13]($9, $14, $15, $16, 0, 0, $19, $22, $24, $26); //@line 47 "ddwrap.cpp"
  return $27; //@line 47 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass7bltFromERS_iiiiiibb($this, $srf, $xD, $yD, $xS, $yS, $wS, $hS, $clip, $transparent) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  var $6;
  var $7;
  var $8;
  var $9;
  var $10;
  $1=$this;
  $2=$srf;
  $3=$xD;
  $4=$yD;
  $5=$xS;
  $6=$yS;
  $7=$wS;
  $8=$hS;
  var $11=(($clip)&(1));
  $9=$11;
  var $12=(($transparent)&(1));
  $10=$12;
  var $13=$1;
  var $14=$13; //@line 64 "ddwrap.cpp"
  var $15=HEAP32[(($14)>>2)]; //@line 64 "ddwrap.cpp"
  var $16=(($15+16)|0); //@line 64 "ddwrap.cpp"
  var $17=HEAP32[(($16)>>2)]; //@line 64 "ddwrap.cpp"
  var $18=$2; //@line 64 "ddwrap.cpp"
  var $19=$3; //@line 64 "ddwrap.cpp"
  var $20=$4; //@line 64 "ddwrap.cpp"
  var $21=$5; //@line 64 "ddwrap.cpp"
  var $22=$6; //@line 64 "ddwrap.cpp"
  var $23=$7; //@line 64 "ddwrap.cpp"
  var $24=$8; //@line 64 "ddwrap.cpp"
  var $25=(($13+4)|0); //@line 64 "ddwrap.cpp"
  var $26=HEAP32[(($25)>>2)]; //@line 64 "ddwrap.cpp"
  var $27=(($13+8)|0); //@line 64 "ddwrap.cpp"
  var $28=HEAP32[(($27)>>2)]; //@line 64 "ddwrap.cpp"
  var $29=$9; //@line 64 "ddwrap.cpp"
  var $30=(($29) & 1); //@line 64 "ddwrap.cpp"
  var $31=$10; //@line 64 "ddwrap.cpp"
  var $32=(($31) & 1); //@line 64 "ddwrap.cpp"
  var $33=FUNCTION_TABLE[$17]($13, $18, $19, $20, $21, $22, $23, $24, 0, 0, $26, $28, $30, $32); //@line 64 "ddwrap.cpp"
  return $33; //@line 64 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass7bltFromERS_iiiiiiiiiibb($this, $srf, $xD, $yD, $xS, $yS, $wS, $hS, $xClip, $yClip, $wClip, $hClip, $clip, $transparent) {
  var label = 0;
  var __stackBase__  = STACKTOP; STACKTOP = (STACKTOP + 64)|0; assert((STACKTOP|0) < (STACK_MAX|0));
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $5;
      var $6;
      var $7;
      var $8;
      var $9;
      var $10;
      var $11;
      var $12;
      var $13;
      var $14;
      var $15;
      var $rectS=__stackBase__;
      var $rectD=(__stackBase__)+(16);
      var $rectS2=(__stackBase__)+(32);
      var $rectD2=(__stackBase__)+(48);
      $2=$this;
      $3=$srf;
      $4=$xD;
      $5=$yD;
      $6=$xS;
      $7=$yS;
      $8=$wS;
      $9=$hS;
      $10=$xClip;
      $11=$yClip;
      $12=$wClip;
      $13=$hClip;
      var $16=(($clip)&(1));
      $14=$16;
      var $17=(($transparent)&(1));
      $15=$17;
      var $18=$2;
      var $19=$18; //@line 87 "ddwrap.cpp"
      var $20=HEAP32[(($19)>>2)]; //@line 87 "ddwrap.cpp"
      var $21=(($20+104)|0); //@line 87 "ddwrap.cpp"
      var $22=HEAP32[(($21)>>2)]; //@line 87 "ddwrap.cpp"
      var $23=FUNCTION_TABLE[$22]($18); //@line 87 "ddwrap.cpp"
      if ($23) { label = 2; break; } else { label = 3; break; } //@line 87 "ddwrap.cpp"
    case 2: 
      var $25=$3; //@line 87 "ddwrap.cpp"
      var $26=$25; //@line 87 "ddwrap.cpp"
      var $27=HEAP32[(($26)>>2)]; //@line 87 "ddwrap.cpp"
      var $28=(($27+104)|0); //@line 87 "ddwrap.cpp"
      var $29=HEAP32[(($28)>>2)]; //@line 87 "ddwrap.cpp"
      var $30=FUNCTION_TABLE[$29]($25); //@line 87 "ddwrap.cpp"
      if ($30) { label = 4; break; } else { label = 3; break; } //@line 87 "ddwrap.cpp"
    case 3: 
      $1=0; //@line 87 "ddwrap.cpp"
      label = 22; break; //@line 87 "ddwrap.cpp"
    case 4: 
      var $33=$6; //@line 89 "ddwrap.cpp"
      var $34=(($rectS)|0); //@line 89 "ddwrap.cpp"
      HEAP32[(($34)>>2)]=$33; //@line 89 "ddwrap.cpp"
      var $35=$7; //@line 90 "ddwrap.cpp"
      var $36=(($rectS+4)|0); //@line 90 "ddwrap.cpp"
      HEAP32[(($36)>>2)]=$35; //@line 90 "ddwrap.cpp"
      var $37=$6; //@line 91 "ddwrap.cpp"
      var $38=$8; //@line 91 "ddwrap.cpp"
      var $39=((($37)+($38))|0); //@line 91 "ddwrap.cpp"
      var $40=(($rectS+8)|0); //@line 91 "ddwrap.cpp"
      HEAP32[(($40)>>2)]=$39; //@line 91 "ddwrap.cpp"
      var $41=$7; //@line 92 "ddwrap.cpp"
      var $42=$9; //@line 92 "ddwrap.cpp"
      var $43=((($41)+($42))|0); //@line 92 "ddwrap.cpp"
      var $44=(($rectS+12)|0); //@line 92 "ddwrap.cpp"
      HEAP32[(($44)>>2)]=$43; //@line 92 "ddwrap.cpp"
      var $45=$14; //@line 94 "ddwrap.cpp"
      var $46=(($45) & 1); //@line 94 "ddwrap.cpp"
      if ($46) { label = 5; break; } else { label = 19; break; } //@line 94 "ddwrap.cpp"
    case 5: 
      var $48=$4; //@line 96 "ddwrap.cpp"
      var $49=$8; //@line 96 "ddwrap.cpp"
      var $50=((($48)+($49))|0); //@line 96 "ddwrap.cpp"
      var $51=$10; //@line 96 "ddwrap.cpp"
      var $52=(($50)|(0)) <= (($51)|(0)); //@line 96 "ddwrap.cpp"
      if ($52) { label = 9; break; } else { label = 6; break; } //@line 96 "ddwrap.cpp"
    case 6: 
      var $54=$5; //@line 96 "ddwrap.cpp"
      var $55=$9; //@line 96 "ddwrap.cpp"
      var $56=((($54)+($55))|0); //@line 96 "ddwrap.cpp"
      var $57=$11; //@line 96 "ddwrap.cpp"
      var $58=(($56)|(0)) <= (($57)|(0)); //@line 96 "ddwrap.cpp"
      if ($58) { label = 9; break; } else { label = 7; break; } //@line 96 "ddwrap.cpp"
    case 7: 
      var $60=$4; //@line 96 "ddwrap.cpp"
      var $61=$10; //@line 96 "ddwrap.cpp"
      var $62=$12; //@line 96 "ddwrap.cpp"
      var $63=((($61)+($62))|0); //@line 96 "ddwrap.cpp"
      var $64=(($60)|(0)) >= (($63)|(0)); //@line 96 "ddwrap.cpp"
      if ($64) { label = 9; break; } else { label = 8; break; } //@line 96 "ddwrap.cpp"
    case 8: 
      var $66=$5; //@line 96 "ddwrap.cpp"
      var $67=$11; //@line 96 "ddwrap.cpp"
      var $68=$13; //@line 96 "ddwrap.cpp"
      var $69=((($67)+($68))|0); //@line 96 "ddwrap.cpp"
      var $70=(($66)|(0)) >= (($69)|(0)); //@line 96 "ddwrap.cpp"
      if ($70) { label = 9; break; } else { label = 10; break; } //@line 96 "ddwrap.cpp"
    case 9: 
      $1=1; //@line 98 "ddwrap.cpp"
      label = 22; break; //@line 98 "ddwrap.cpp"
    case 10: 
      var $73=$4; //@line 101 "ddwrap.cpp"
      var $74=$10; //@line 101 "ddwrap.cpp"
      var $75=(($73)|(0)) < (($74)|(0)); //@line 101 "ddwrap.cpp"
      if ($75) { label = 11; break; } else { label = 12; break; } //@line 101 "ddwrap.cpp"
    case 11: 
      var $77=(($rectS)|0); //@line 103 "ddwrap.cpp"
      var $78=HEAP32[(($77)>>2)]; //@line 103 "ddwrap.cpp"
      var $79=$4; //@line 103 "ddwrap.cpp"
      var $80=((($78)-($79))|0); //@line 103 "ddwrap.cpp"
      var $81=$10; //@line 103 "ddwrap.cpp"
      var $82=((($80)+($81))|0); //@line 103 "ddwrap.cpp"
      var $83=(($rectS)|0); //@line 103 "ddwrap.cpp"
      HEAP32[(($83)>>2)]=$82; //@line 103 "ddwrap.cpp"
      var $84=$10; //@line 104 "ddwrap.cpp"
      $4=$84; //@line 104 "ddwrap.cpp"
      label = 12; break; //@line 105 "ddwrap.cpp"
    case 12: 
      var $86=$5; //@line 107 "ddwrap.cpp"
      var $87=$11; //@line 107 "ddwrap.cpp"
      var $88=(($86)|(0)) < (($87)|(0)); //@line 107 "ddwrap.cpp"
      if ($88) { label = 13; break; } else { label = 14; break; } //@line 107 "ddwrap.cpp"
    case 13: 
      var $90=(($rectS+4)|0); //@line 109 "ddwrap.cpp"
      var $91=HEAP32[(($90)>>2)]; //@line 109 "ddwrap.cpp"
      var $92=$5; //@line 109 "ddwrap.cpp"
      var $93=((($91)-($92))|0); //@line 109 "ddwrap.cpp"
      var $94=$11; //@line 109 "ddwrap.cpp"
      var $95=((($93)+($94))|0); //@line 109 "ddwrap.cpp"
      var $96=(($rectS+4)|0); //@line 109 "ddwrap.cpp"
      HEAP32[(($96)>>2)]=$95; //@line 109 "ddwrap.cpp"
      var $97=$11; //@line 110 "ddwrap.cpp"
      $5=$97; //@line 110 "ddwrap.cpp"
      label = 14; break; //@line 111 "ddwrap.cpp"
    case 14: 
      var $99=$4; //@line 113 "ddwrap.cpp"
      var $100=$8; //@line 113 "ddwrap.cpp"
      var $101=((($99)+($100))|0); //@line 113 "ddwrap.cpp"
      var $102=$10; //@line 113 "ddwrap.cpp"
      var $103=$12; //@line 113 "ddwrap.cpp"
      var $104=((($102)+($103))|0); //@line 113 "ddwrap.cpp"
      var $105=(($101)|(0)) > (($104)|(0)); //@line 113 "ddwrap.cpp"
      if ($105) { label = 15; break; } else { label = 16; break; } //@line 113 "ddwrap.cpp"
    case 15: 
      var $107=(($rectS)|0); //@line 115 "ddwrap.cpp"
      var $108=HEAP32[(($107)>>2)]; //@line 115 "ddwrap.cpp"
      var $109=$10; //@line 115 "ddwrap.cpp"
      var $110=((($108)+($109))|0); //@line 115 "ddwrap.cpp"
      var $111=$12; //@line 115 "ddwrap.cpp"
      var $112=((($110)+($111))|0); //@line 115 "ddwrap.cpp"
      var $113=$4; //@line 115 "ddwrap.cpp"
      var $114=((($112)-($113))|0); //@line 115 "ddwrap.cpp"
      var $115=(($rectS+8)|0); //@line 115 "ddwrap.cpp"
      HEAP32[(($115)>>2)]=$114; //@line 115 "ddwrap.cpp"
      label = 16; break; //@line 116 "ddwrap.cpp"
    case 16: 
      var $117=$5; //@line 118 "ddwrap.cpp"
      var $118=$9; //@line 118 "ddwrap.cpp"
      var $119=((($117)+($118))|0); //@line 118 "ddwrap.cpp"
      var $120=$11; //@line 118 "ddwrap.cpp"
      var $121=$13; //@line 118 "ddwrap.cpp"
      var $122=((($120)+($121))|0); //@line 118 "ddwrap.cpp"
      var $123=(($119)|(0)) > (($122)|(0)); //@line 118 "ddwrap.cpp"
      if ($123) { label = 17; break; } else { label = 18; break; } //@line 118 "ddwrap.cpp"
    case 17: 
      var $125=(($rectS+4)|0); //@line 120 "ddwrap.cpp"
      var $126=HEAP32[(($125)>>2)]; //@line 120 "ddwrap.cpp"
      var $127=$11; //@line 120 "ddwrap.cpp"
      var $128=((($126)+($127))|0); //@line 120 "ddwrap.cpp"
      var $129=$13; //@line 120 "ddwrap.cpp"
      var $130=((($128)+($129))|0); //@line 120 "ddwrap.cpp"
      var $131=$5; //@line 120 "ddwrap.cpp"
      var $132=((($130)-($131))|0); //@line 120 "ddwrap.cpp"
      var $133=(($rectS+12)|0); //@line 120 "ddwrap.cpp"
      HEAP32[(($133)>>2)]=$132; //@line 120 "ddwrap.cpp"
      label = 18; break; //@line 121 "ddwrap.cpp"
    case 18: 
      label = 19; break; //@line 122 "ddwrap.cpp"
    case 19: 
      var $136=$4; //@line 136 "ddwrap.cpp"
      var $137=(($rectD)|0); //@line 136 "ddwrap.cpp"
      HEAP32[(($137)>>2)]=$136; //@line 136 "ddwrap.cpp"
      var $138=$5; //@line 137 "ddwrap.cpp"
      var $139=(($rectD+4)|0); //@line 137 "ddwrap.cpp"
      HEAP32[(($139)>>2)]=$138; //@line 137 "ddwrap.cpp"
      var $140=(($rectD+4)|0); //@line 138 "ddwrap.cpp"
      var $141=HEAP32[(($140)>>2)]; //@line 138 "ddwrap.cpp"
      var $142=(($rectS+12)|0); //@line 138 "ddwrap.cpp"
      var $143=HEAP32[(($142)>>2)]; //@line 138 "ddwrap.cpp"
      var $144=((($141)+($143))|0); //@line 138 "ddwrap.cpp"
      var $145=(($rectS+4)|0); //@line 138 "ddwrap.cpp"
      var $146=HEAP32[(($145)>>2)]; //@line 138 "ddwrap.cpp"
      var $147=((($144)-($146))|0); //@line 138 "ddwrap.cpp"
      var $148=(($rectD+12)|0); //@line 138 "ddwrap.cpp"
      HEAP32[(($148)>>2)]=$147; //@line 138 "ddwrap.cpp"
      var $149=(($rectD)|0); //@line 139 "ddwrap.cpp"
      var $150=HEAP32[(($149)>>2)]; //@line 139 "ddwrap.cpp"
      var $151=(($rectS+8)|0); //@line 139 "ddwrap.cpp"
      var $152=HEAP32[(($151)>>2)]; //@line 139 "ddwrap.cpp"
      var $153=((($150)+($152))|0); //@line 139 "ddwrap.cpp"
      var $154=(($rectS)|0); //@line 139 "ddwrap.cpp"
      var $155=HEAP32[(($154)>>2)]; //@line 139 "ddwrap.cpp"
      var $156=((($153)-($155))|0); //@line 139 "ddwrap.cpp"
      var $157=(($rectD+8)|0); //@line 139 "ddwrap.cpp"
      HEAP32[(($157)>>2)]=$156; //@line 139 "ddwrap.cpp"
      var $158=(($rectS)|0); //@line 144 "ddwrap.cpp"
      var $159=HEAP32[(($158)>>2)]; //@line 144 "ddwrap.cpp"
      var $160=(($rectS2)|0); //@line 144 "ddwrap.cpp"
      HEAP32[(($160)>>2)]=$159; //@line 144 "ddwrap.cpp"
      var $161=(($rectS+4)|0); //@line 144 "ddwrap.cpp"
      var $162=HEAP32[(($161)>>2)]; //@line 144 "ddwrap.cpp"
      var $163=(($rectS2+4)|0); //@line 144 "ddwrap.cpp"
      HEAP32[(($163)>>2)]=$162; //@line 144 "ddwrap.cpp"
      var $164=(($rectS+8)|0); //@line 144 "ddwrap.cpp"
      var $165=HEAP32[(($164)>>2)]; //@line 144 "ddwrap.cpp"
      var $166=(($rectS)|0); //@line 144 "ddwrap.cpp"
      var $167=HEAP32[(($166)>>2)]; //@line 144 "ddwrap.cpp"
      var $168=((($165)-($167))|0); //@line 144 "ddwrap.cpp"
      var $169=(($rectS2+8)|0); //@line 144 "ddwrap.cpp"
      HEAP32[(($169)>>2)]=$168; //@line 144 "ddwrap.cpp"
      var $170=(($rectS+12)|0); //@line 144 "ddwrap.cpp"
      var $171=HEAP32[(($170)>>2)]; //@line 144 "ddwrap.cpp"
      var $172=(($rectS+4)|0); //@line 144 "ddwrap.cpp"
      var $173=HEAP32[(($172)>>2)]; //@line 144 "ddwrap.cpp"
      var $174=((($171)-($173))|0); //@line 144 "ddwrap.cpp"
      var $175=(($rectS2+12)|0); //@line 144 "ddwrap.cpp"
      HEAP32[(($175)>>2)]=$174; //@line 144 "ddwrap.cpp"
      var $176=(($rectD)|0); //@line 145 "ddwrap.cpp"
      var $177=HEAP32[(($176)>>2)]; //@line 145 "ddwrap.cpp"
      var $178=(($rectD2)|0); //@line 145 "ddwrap.cpp"
      HEAP32[(($178)>>2)]=$177; //@line 145 "ddwrap.cpp"
      var $179=(($rectD+4)|0); //@line 145 "ddwrap.cpp"
      var $180=HEAP32[(($179)>>2)]; //@line 145 "ddwrap.cpp"
      var $181=(($rectD2+4)|0); //@line 145 "ddwrap.cpp"
      HEAP32[(($181)>>2)]=$180; //@line 145 "ddwrap.cpp"
      var $182=(($rectD+8)|0); //@line 145 "ddwrap.cpp"
      var $183=HEAP32[(($182)>>2)]; //@line 145 "ddwrap.cpp"
      var $184=(($rectD)|0); //@line 145 "ddwrap.cpp"
      var $185=HEAP32[(($184)>>2)]; //@line 145 "ddwrap.cpp"
      var $186=((($183)-($185))|0); //@line 145 "ddwrap.cpp"
      var $187=(($rectD2+8)|0); //@line 145 "ddwrap.cpp"
      HEAP32[(($187)>>2)]=$186; //@line 145 "ddwrap.cpp"
      var $188=(($rectD+12)|0); //@line 145 "ddwrap.cpp"
      var $189=HEAP32[(($188)>>2)]; //@line 145 "ddwrap.cpp"
      var $190=(($rectD+4)|0); //@line 145 "ddwrap.cpp"
      var $191=HEAP32[(($190)>>2)]; //@line 145 "ddwrap.cpp"
      var $192=((($189)-($191))|0); //@line 145 "ddwrap.cpp"
      var $193=(($rectD2+12)|0); //@line 145 "ddwrap.cpp"
      HEAP32[(($193)>>2)]=$192; //@line 145 "ddwrap.cpp"
      var $194=$3; //@line 147 "ddwrap.cpp"
      var $195=(($194+32)|0); //@line 147 "ddwrap.cpp"
      var $196=HEAP32[(($195)>>2)]; //@line 147 "ddwrap.cpp"
      var $197=(($18+32)|0); //@line 147 "ddwrap.cpp"
      var $198=HEAP32[(($197)>>2)]; //@line 147 "ddwrap.cpp"
      var $199=_SDL_UpperBlit($196, $rectS2, $198, $rectD2); //@line 147 "ddwrap.cpp"
      var $200=(($199)|(0))!=0; //@line 147 "ddwrap.cpp"
      if ($200) { label = 20; break; } else { label = 21; break; } //@line 147 "ddwrap.cpp"
    case 20: 
      $1=0; //@line 153 "ddwrap.cpp"
      label = 22; break; //@line 153 "ddwrap.cpp"
    case 21: 
      $1=1; //@line 156 "ddwrap.cpp"
      label = 22; break; //@line 156 "ddwrap.cpp"
    case 22: 
      var $204=$1; //@line 157 "ddwrap.cpp"
      STACKTOP = __stackBase__;
      return $204; //@line 157 "ddwrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN16loopWrapperClassD2Ev($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      $1=$this;
      var $4=$1;
      var $5=$4; //@line 60 "./loopwrap.h"
      HEAP32[(($5)>>2)]=((912)|0); //@line 60 "./loopwrap.h"
      var $6=$4; //@line 60 "./loopwrap.h"
      var $7=HEAP32[(($6)>>2)]; //@line 60 "./loopwrap.h"
      var $8=(($7+12)|0); //@line 60 "./loopwrap.h"
      var $9=HEAP32[(($8)>>2)]; //@line 60 "./loopwrap.h"
      var $10 = (function() { try { __THREW__ = 0; return FUNCTION_TABLE[$9]($4) } catch(e) { if (typeof e != "number") throw e; if (ABORT) throw e; __THREW__ = 1; return null } })();if (!__THREW__) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $12=(($4+8)|0); //@line 60 "./loopwrap.h"
      __ZN16timeWrapperClassD1Ev($12); //@line 60 "./loopwrap.h"
      return; //@line 60 "./loopwrap.h"
    case 3: 
      var $14$0 = ___cxa_find_matching_catch(-1, -1); $14$1 = tempRet0;
      var $15=$14$0;
      $2=$15; //@line 60 "./loopwrap.h"
      var $16=$14$1;
      $3=$16; //@line 60 "./loopwrap.h"
      var $17=(($4+8)|0); //@line 60 "./loopwrap.h"
      (function() { try { __THREW__ = 0; return __ZN16timeWrapperClassD1Ev($17) } catch(e) { if (typeof e != "number") throw e; if (ABORT) throw e; __THREW__ = 1; return null } })();if (!__THREW__) { label = 4; break; } else { label = 6; break; }
    case 4: 
      label = 5; break; //@line 60 "./loopwrap.h"
    case 5: 
      var $20=$2; //@line 60 "./loopwrap.h"
      var $21=$3; //@line 60 "./loopwrap.h"
      var $22$0=$20;
      var $22$1=0;
      var $23$0=$22$0;
      var $23$1=$21;
      ___resumeException($23$0) //@line 60 "./loopwrap.h"
    case 6: 
      var $25$0 = ___cxa_find_matching_catch(-1, -1,0); $25$1 = tempRet0;
      __ZSt9terminatev(); //@line 60 "./loopwrap.h"
      throw "Reached an unreachable!"; //@line 60 "./loopwrap.h"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN15directDrawClassD2Ev($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      $1=$this;
      var $4=$1;
      var $5=$4; //@line 223 "./ddwrap.h"
      HEAP32[(($5)>>2)]=((984)|0); //@line 223 "./ddwrap.h"
      var $6 = (function() { try { __THREW__ = 0; return __ZN15directDrawClass3endEv($4) } catch(e) { if (typeof e != "number") throw e; if (ABORT) throw e; __THREW__ = 1; return null } })();if (!__THREW__) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $8=$4; //@line 223 "./ddwrap.h"
      __ZN14ddSurfaceClassD2Ev($8); //@line 223 "./ddwrap.h"
      return; //@line 223 "./ddwrap.h"
    case 3: 
      var $10$0 = ___cxa_find_matching_catch(-1, -1); $10$1 = tempRet0;
      var $11=$10$0;
      $2=$11; //@line 223 "./ddwrap.h"
      var $12=$10$1;
      $3=$12; //@line 223 "./ddwrap.h"
      var $13=$4; //@line 223 "./ddwrap.h"
      (function() { try { __THREW__ = 0; return __ZN14ddSurfaceClassD2Ev($13) } catch(e) { if (typeof e != "number") throw e; if (ABORT) throw e; __THREW__ = 1; return null } })();if (!__THREW__) { label = 4; break; } else { label = 6; break; }
    case 4: 
      label = 5; break; //@line 223 "./ddwrap.h"
    case 5: 
      var $16=$2; //@line 223 "./ddwrap.h"
      var $17=$3; //@line 223 "./ddwrap.h"
      var $18$0=$16;
      var $18$1=0;
      var $19$0=$18$0;
      var $19$1=$17;
      ___resumeException($19$0) //@line 223 "./ddwrap.h"
    case 6: 
      var $21$0 = ___cxa_find_matching_catch(-1, -1,0); $21$1 = tempRet0;
      __ZSt9terminatev(); //@line 223 "./ddwrap.h"
      throw "Reached an unreachable!"; //@line 223 "./ddwrap.h"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN14ddSurfaceClass8putPixelEiiiii($this, $x, $y, $r, $g, $b) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  var $6;
  $1=$this;
  $2=$x;
  $3=$y;
  $4=$r;
  $5=$g;
  $6=$b;
  var $7=$1;
  return 1; //@line 628 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass8drawLineEiiiiiii($this, $x, $y, $x2, $y2, $r, $g, $b) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  var $6;
  var $7;
  var $8;
  $1=$this;
  $2=$x;
  $3=$y;
  $4=$x2;
  $5=$y2;
  $6=$r;
  $7=$g;
  $8=$b;
  var $9=$1;
  return 1; //@line 691 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass8fillRectEiiiiiii($this, $r, $g, $b, $x, $y, $width, $height) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  var $6;
  var $7;
  var $8;
  $1=$this;
  $2=$r;
  $3=$g;
  $4=$b;
  $5=$x;
  $6=$y;
  $7=$width;
  $8=$height;
  var $9=$1;
  return 1; //@line 744 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass12fillAllDwordEm($this, $color) {
  var label = 0;
  var $1;
  var $2;
  $1=$this;
  $2=$color;
  var $3=$1;
  return 1; //@line 771 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass9writeTextEPciii9alignEnumiiii($this, $text, $r, $g, $b, $align, $x, $y, $width, $height) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  var $6;
  var $7;
  var $8;
  var $9;
  var $10;
  $1=$this;
  $2=$text;
  $3=$r;
  $4=$g;
  $5=$b;
  $6=$align;
  $7=$x;
  $8=$y;
  $9=$width;
  $10=$height;
  var $11=$1;
  return 1; //@line 851 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass13calculateTextEPc9alignEnumRiS2_($this, $text, $align, $width, $height) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  $1=$this;
  $2=$text;
  $3=$align;
  $4=$width;
  $5=$height;
  var $6=$1;
  return 1; //@line 936 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass6unlockEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  return 1; //@line 1278 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass17getDwordFromPixelEiiRm($this, $x, $y, $color) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  $1=$this;
  $2=$x;
  $3=$y;
  $4=$color;
  var $5=$1;
  return 1; //@line 1312 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass15getDwordFromRGBEiiiRm($this, $r, $g, $b, $color) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  $1=$this;
  $2=$r;
  $3=$g;
  $4=$b;
  $5=$color;
  var $6=$1;
  return 1; //@line 1348 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass6isLostEPb($this, $errorCode) {
  var label = 0;
  var $1;
  var $2;
  $1=$this;
  $2=$errorCode;
  var $3=$1;
  return 0; //@line 1417 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass7restoreEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  return 1; //@line 1428 "ddwrap.cpp"
}
function __ZN15directDrawClass6isLostEPb($this, $errorCode) {
  var label = 0;
  var $1;
  var $2;
  $1=$this;
  $2=$errorCode;
  var $3=$1;
  return 0; //@line 1707 "ddwrap.cpp"
}
function __ZN15directDrawClass7restoreEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  return 1; //@line 1718 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass11getDDObjectEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=HEAP32[((1672)>>2)]; //@line 178 "./ddwrap.h"
  return $3; //@line 178 "./ddwrap.h"
}
function __ZN14ddSurfaceClass14stretchBltFromERS_iiiiiiiibbb($this, $srf, $xD, $yD, $wD, $hD, $xS, $yS, $wS, $hS, $mirrorX, $mirrorY, $transparent) {
  var label = 0;
  var __stackBase__  = STACKTOP; STACKTOP = (STACKTOP + 32)|0; assert((STACKTOP|0) < (STACK_MAX|0));
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  var $6;
  var $7;
  var $8;
  var $9;
  var $10;
  var $11;
  var $12;
  var $13;
  var $rectS=__stackBase__;
  var $rectD=(__stackBase__)+(16);
  $1=$this;
  $2=$srf;
  $3=$xD;
  $4=$yD;
  $5=$wD;
  $6=$hD;
  $7=$xS;
  $8=$yS;
  $9=$wS;
  $10=$hS;
  var $14=(($mirrorX)&(1));
  $11=$14;
  var $15=(($mirrorY)&(1));
  $12=$15;
  var $16=(($transparent)&(1));
  $13=$16;
  var $17=$1;
  STACKTOP = __stackBase__;
  return 1; //@line 211 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass12alphaBltFromERS_iiib($this, $srfSrc, $xD, $yD, $alphaValue, $transparent) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  var $6;
  $1=$this;
  $2=$srfSrc;
  $3=$xD;
  $4=$yD;
  $5=$alphaValue;
  var $7=(($transparent)&(1));
  $6=$7;
  var $8=$1;
  return 1; //@line 603 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass13saveAsBmpFileEPcb($this, $filename, $overwrite) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  $1=$this;
  $2=$filename;
  var $4=(($overwrite)&(1));
  $3=$4;
  var $5=$1;
  return 1; //@line 1196 "ddwrap.cpp"
}
function __ZN14ddSurfaceClass4lockEbbPmPPv($this, $onlyRead, $onlyWrite, $pitch, $pointer) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  $1=$this;
  var $6=(($onlyRead)&(1));
  $2=$6;
  var $7=(($onlyWrite)&(1));
  $3=$7;
  $4=$pitch;
  $5=$pointer;
  var $8=$1;
  return 1; //@line 1263 "ddwrap.cpp"
}
function __ZN15directDrawClass4initEv($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      $2=$this;
      var $3=$2;
      var $4=HEAP8[(1664)]; //@line 1436 "ddwrap.cpp"
      var $5=(($4) & 1); //@line 1436 "ddwrap.cpp"
      if ($5) { label = 2; break; } else { label = 3; break; } //@line 1436 "ddwrap.cpp"
    case 2: 
      $1=0; //@line 1436 "ddwrap.cpp"
      label = 4; break; //@line 1436 "ddwrap.cpp"
    case 3: 
      HEAP8[(1664)]=1; //@line 1439 "ddwrap.cpp"
      var $8=(($3+36)|0); //@line 1442 "ddwrap.cpp"
      HEAP8[($8)]=1; //@line 1442 "ddwrap.cpp"
      var $9=(($3+37)|0); //@line 1443 "ddwrap.cpp"
      HEAP8[($9)]=0; //@line 1443 "ddwrap.cpp"
      $1=1; //@line 1445 "ddwrap.cpp"
      label = 4; break; //@line 1445 "ddwrap.cpp"
    case 4: 
      var $11=$1; //@line 1446 "ddwrap.cpp"
      return $11; //@line 1446 "ddwrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN15directDrawClass14closeVideoModeEv($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      $2=$this;
      var $3=$2;
      var $4=(($3+37)|0); //@line 1473 "ddwrap.cpp"
      var $5=HEAP8[($4)]; //@line 1473 "ddwrap.cpp"
      var $6=(($5) & 1); //@line 1473 "ddwrap.cpp"
      if ($6) { label = 3; break; } else { label = 2; break; } //@line 1473 "ddwrap.cpp"
    case 2: 
      $1=0; //@line 1473 "ddwrap.cpp"
      label = 4; break; //@line 1473 "ddwrap.cpp"
    case 3: 
      var $9=(($3+37)|0); //@line 1479 "ddwrap.cpp"
      HEAP8[($9)]=0; //@line 1479 "ddwrap.cpp"
      $1=1; //@line 1481 "ddwrap.cpp"
      label = 4; break; //@line 1481 "ddwrap.cpp"
    case 4: 
      var $11=$1; //@line 1482 "ddwrap.cpp"
      return $11; //@line 1482 "ddwrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN15directDrawClass7isReadyEv($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      $1=$this;
      var $2=$1;
      var $3=(($2+36)|0); //@line 1692 "ddwrap.cpp"
      var $4=HEAP8[($3)]; //@line 1692 "ddwrap.cpp"
      var $5=(($4) & 1); //@line 1692 "ddwrap.cpp"
      if ($5) { label = 2; break; } else { var $11 = 0;label = 3; break; } //@line 1692 "ddwrap.cpp"
    case 2: 
      var $7=(($2+37)|0); //@line 1692 "ddwrap.cpp"
      var $8=HEAP8[($7)]; //@line 1692 "ddwrap.cpp"
      var $9=(($8) & 1); //@line 1692 "ddwrap.cpp"
      var $11 = $9;label = 3; break;
    case 3: 
      var $11;
      return $11; //@line 1693 "ddwrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN14ddSurfaceClass15isInVideoMemoryEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=(($2+20)|0); //@line 167 "./ddwrap.h"
  var $4=HEAP8[($3)]; //@line 167 "./ddwrap.h"
  var $5=(($4) & 1); //@line 167 "./ddwrap.h"
  return $5; //@line 167 "./ddwrap.h"
}
function __ZN14ddSurfaceClass8getWidthEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=(($2+4)|0); //@line 168 "./ddwrap.h"
  var $4=HEAP32[(($3)>>2)]; //@line 168 "./ddwrap.h"
  return $4; //@line 168 "./ddwrap.h"
}
function __ZN14ddSurfaceClass9getHeightEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=(($2+8)|0); //@line 169 "./ddwrap.h"
  var $4=HEAP32[(($3)>>2)]; //@line 169 "./ddwrap.h"
  return $4; //@line 169 "./ddwrap.h"
}
function __ZN14ddSurfaceClass12getRealWidthEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=(($2+12)|0); //@line 170 "./ddwrap.h"
  var $4=HEAP32[(($3)>>2)]; //@line 170 "./ddwrap.h"
  return $4; //@line 170 "./ddwrap.h"
}
function __ZN14ddSurfaceClass13getRealHeightEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=(($2+16)|0); //@line 171 "./ddwrap.h"
  var $4=HEAP32[(($3)>>2)]; //@line 171 "./ddwrap.h"
  return $4; //@line 171 "./ddwrap.h"
}
function __ZN16timeWrapperClass7isReadyEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=(($2+4)|0); //@line 70 "./timewrap.h"
  var $4=HEAP8[($3)]; //@line 70 "./timewrap.h"
  var $5=(($4) & 1); //@line 70 "./timewrap.h"
  return $5; //@line 70 "./timewrap.h"
}
function __ZN14ddSurfaceClass15obscuredBltFromERS_iiib($this, $srfSrc, $xD, $yD, $lightness, $transparent) {
  var label = 0;
  var $1;
  var $2;
  var $3;
  var $4;
  var $5;
  var $6;
  $1=$this;
  $2=$srfSrc;
  $3=$xD;
  $4=$yD;
  $5=$lightness;
  var $7=(($transparent)&(1));
  $6=$7;
  var $8=$1;
  var $9=$8; //@line 230 "ddwrap.cpp"
  var $10=HEAP32[(($9)>>2)]; //@line 230 "ddwrap.cpp"
  var $11=(($10+8)|0); //@line 230 "ddwrap.cpp"
  var $12=HEAP32[(($11)>>2)]; //@line 230 "ddwrap.cpp"
  var $13=$2; //@line 230 "ddwrap.cpp"
  var $14=$3; //@line 230 "ddwrap.cpp"
  var $15=$4; //@line 230 "ddwrap.cpp"
  var $16=$6; //@line 230 "ddwrap.cpp"
  var $17=(($16) & 1); //@line 230 "ddwrap.cpp"
  var $18=FUNCTION_TABLE[$12]($8, $13, $14, $15, 1, $17); //@line 230 "ddwrap.cpp"
  return $18; //@line 230 "ddwrap.cpp"
}
function __ZN15directDrawClass3endEv($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      $2=$this;
      var $3=$2;
      var $4=(($3+36)|0); //@line 1450 "ddwrap.cpp"
      var $5=HEAP8[($4)]; //@line 1450 "ddwrap.cpp"
      var $6=(($5) & 1); //@line 1450 "ddwrap.cpp"
      var $7=(($6)&(1)); //@line 1450 "ddwrap.cpp"
      var $8=(($7)|(0))==0; //@line 1450 "ddwrap.cpp"
      if ($8) { label = 2; break; } else { label = 3; break; } //@line 1450 "ddwrap.cpp"
    case 2: 
      $1=0; //@line 1450 "ddwrap.cpp"
      label = 8; break; //@line 1450 "ddwrap.cpp"
    case 3: 
      var $11=(($3+37)|0); //@line 1453 "ddwrap.cpp"
      var $12=HEAP8[($11)]; //@line 1453 "ddwrap.cpp"
      var $13=(($12) & 1); //@line 1453 "ddwrap.cpp"
      if ($13) { label = 4; break; } else { label = 7; break; } //@line 1453 "ddwrap.cpp"
    case 4: 
      var $15=__ZN15directDrawClass14closeVideoModeEv($3); //@line 1454 "ddwrap.cpp"
      if ($15) { label = 6; break; } else { label = 5; break; } //@line 1454 "ddwrap.cpp"
    case 5: 
      $1=0; //@line 1454 "ddwrap.cpp"
      label = 8; break; //@line 1454 "ddwrap.cpp"
    case 6: 
      label = 7; break; //@line 1455 "ddwrap.cpp"
    case 7: 
      HEAP32[((1688)>>2)]=0; //@line 1458 "ddwrap.cpp"
      var $19=$3; //@line 1459 "ddwrap.cpp"
      var $20=(($19+32)|0); //@line 1459 "ddwrap.cpp"
      HEAP32[(($20)>>2)]=0; //@line 1459 "ddwrap.cpp"
      HEAP8[(1664)]=0; //@line 1462 "ddwrap.cpp"
      var $21=(($3+36)|0); //@line 1465 "ddwrap.cpp"
      HEAP8[($21)]=0; //@line 1465 "ddwrap.cpp"
      var $22=(($3+37)|0); //@line 1466 "ddwrap.cpp"
      HEAP8[($22)]=0; //@line 1466 "ddwrap.cpp"
      $1=1; //@line 1468 "ddwrap.cpp"
      label = 8; break; //@line 1468 "ddwrap.cpp"
    case 8: 
      var $24=$1; //@line 1469 "ddwrap.cpp"
      return $24; //@line 1469 "ddwrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN15directDrawClass12setVideoModeEiiii($this, $x, $y, $bpp, $backbuffers) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $5;
      var $6;
      $2=$this;
      $3=$x;
      $4=$y;
      $5=$bpp;
      $6=$backbuffers;
      var $7=$2;
      var $8=(($7+36)|0); //@line 1486 "ddwrap.cpp"
      var $9=HEAP8[($8)]; //@line 1486 "ddwrap.cpp"
      var $10=(($9) & 1); //@line 1486 "ddwrap.cpp"
      if ($10) { label = 3; break; } else { label = 2; break; } //@line 1486 "ddwrap.cpp"
    case 2: 
      $1=0; //@line 1486 "ddwrap.cpp"
      label = 6; break; //@line 1486 "ddwrap.cpp"
    case 3: 
      var $13=$6; //@line 1488 "ddwrap.cpp"
      var $14=(($13)|(0)) < 1; //@line 1488 "ddwrap.cpp"
      if ($14) { label = 4; break; } else { label = 5; break; } //@line 1488 "ddwrap.cpp"
    case 4: 
      $1=0; //@line 1488 "ddwrap.cpp"
      label = 6; break; //@line 1488 "ddwrap.cpp"
    case 5: 
      var $17=$3; //@line 1490 "ddwrap.cpp"
      var $18=$4; //@line 1490 "ddwrap.cpp"
      var $19=$5; //@line 1490 "ddwrap.cpp"
      var $20=_SDL_SetVideoMode($17, $18, $19, 1048576); //@line 1490 "ddwrap.cpp"
      HEAP32[((1688)>>2)]=$20; //@line 1490 "ddwrap.cpp"
      var $21=HEAP32[((1688)>>2)]; //@line 1491 "ddwrap.cpp"
      var $22=$7; //@line 1491 "ddwrap.cpp"
      var $23=(($22+32)|0); //@line 1491 "ddwrap.cpp"
      HEAP32[(($23)>>2)]=$21; //@line 1491 "ddwrap.cpp"
      var $24=$3; //@line 1493 "ddwrap.cpp"
      var $25=$7; //@line 1493 "ddwrap.cpp"
      var $26=(($25+4)|0); //@line 1493 "ddwrap.cpp"
      HEAP32[(($26)>>2)]=$24; //@line 1493 "ddwrap.cpp"
      var $27=$4; //@line 1493 "ddwrap.cpp"
      var $28=$7; //@line 1493 "ddwrap.cpp"
      var $29=(($28+8)|0); //@line 1493 "ddwrap.cpp"
      HEAP32[(($29)>>2)]=$27; //@line 1493 "ddwrap.cpp"
      var $30=$5; //@line 1493 "ddwrap.cpp"
      HEAP32[((1680)>>2)]=$30; //@line 1493 "ddwrap.cpp"
      var $31=(($7+37)|0); //@line 1495 "ddwrap.cpp"
      HEAP8[($31)]=1; //@line 1495 "ddwrap.cpp"
      $1=1; //@line 1505 "ddwrap.cpp"
      label = 6; break; //@line 1505 "ddwrap.cpp"
    case 6: 
      var $33=$1; //@line 1506 "ddwrap.cpp"
      return $33; //@line 1506 "ddwrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN15directDrawClass4flipEv($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=HEAP32[((1688)>>2)]; //@line 1684 "ddwrap.cpp"
  _SDL_UpdateRect($3, 0, 0, 0, 0); //@line 1684 "ddwrap.cpp"
  return 1; //@line 1686 "ddwrap.cpp"
}
function __ZN14ddSurfaceClassD1Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN14ddSurfaceClassD2Ev($2); //@line 128 "./ddwrap.h"
  return; //@line 128 "./ddwrap.h"
}
function __ZN14ddSurfaceClassD0Ev($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      $1=$this;
      var $4=$1;
      (function() { try { __THREW__ = 0; return __ZN14ddSurfaceClassD1Ev($4) } catch(e) { if (typeof e != "number") throw e; if (ABORT) throw e; __THREW__ = 1; return null } })();if (!__THREW__) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $6=$4; //@line 128 "./ddwrap.h"
      __ZdlPv($6); //@line 128 "./ddwrap.h"
      return; //@line 128 "./ddwrap.h"
    case 3: 
      var $8$0 = ___cxa_find_matching_catch(-1, -1); $8$1 = tempRet0;
      var $9=$8$0;
      $2=$9; //@line 128 "./ddwrap.h"
      var $10=$8$1;
      $3=$10; //@line 128 "./ddwrap.h"
      var $11=$4; //@line 128 "./ddwrap.h"
      __ZdlPv($11); //@line 128 "./ddwrap.h"
      label = 4; break; //@line 128 "./ddwrap.h"
    case 4: 
      var $13=$2; //@line 128 "./ddwrap.h"
      var $14=$3; //@line 128 "./ddwrap.h"
      var $15$0=$13;
      var $15$1=0;
      var $16$0=$15$0;
      var $16$1=$14;
      ___resumeException($16$0) //@line 128 "./ddwrap.h"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN15directDrawClassD0Ev($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      $1=$this;
      var $4=$1;
      (function() { try { __THREW__ = 0; return __ZN15directDrawClassD1Ev($4) } catch(e) { if (typeof e != "number") throw e; if (ABORT) throw e; __THREW__ = 1; return null } })();if (!__THREW__) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $6=$4; //@line 223 "./ddwrap.h"
      __ZdlPv($6); //@line 223 "./ddwrap.h"
      return; //@line 223 "./ddwrap.h"
    case 3: 
      var $8$0 = ___cxa_find_matching_catch(-1, -1); $8$1 = tempRet0;
      var $9=$8$0;
      $2=$9; //@line 223 "./ddwrap.h"
      var $10=$8$1;
      $3=$10; //@line 223 "./ddwrap.h"
      var $11=$4; //@line 223 "./ddwrap.h"
      __ZdlPv($11); //@line 223 "./ddwrap.h"
      label = 4; break; //@line 223 "./ddwrap.h"
    case 4: 
      var $13=$2; //@line 223 "./ddwrap.h"
      var $14=$3; //@line 223 "./ddwrap.h"
      var $15$0=$13;
      var $15$1=0;
      var $16$0=$15$0;
      var $16$1=$14;
      ___resumeException($16$0) //@line 223 "./ddwrap.h"
    default: assert(0, "bad label: " + label);
  }
}
function _main($argc, $argv) {
  var label = 0;
  var __stackBase__  = STACKTOP; assert((STACKTOP|0) < (STACK_MAX|0));
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      $1=0;
      $2=$argc;
      $3=$argv;
      var $4=_printf(((264)|0), (tempInt=STACKTOP,STACKTOP = (STACKTOP + 1)|0,STACKTOP = ((((STACKTOP)+7)>>3)<<3),assert((STACKTOP|0) < (STACK_MAX|0)),HEAP32[((tempInt)>>2)]=0,tempInt)); //@line 540 "main.cpp"
      var $5=__Z15initializeFirstv(); //@line 575 "main.cpp"
      if ($5) { label = 3; break; } else { label = 2; break; } //@line 575 "main.cpp"
    case 2: 
      $1=0; //@line 576 "main.cpp"
      label = 7; break; //@line 576 "main.cpp"
    case 3: 
      var $8=__Z19switchToProgramZone16programZonesEnummmb(2, 0, 0, 1); //@line 584 "main.cpp"
      if ($8) { label = 5; break; } else { label = 4; break; } //@line 584 "main.cpp"
    case 4: 
      var $10=__Z8closeAllPc(((288)|0)); //@line 586 "main.cpp"
      $1=0; //@line 587 "main.cpp"
      label = 7; break; //@line 587 "main.cpp"
    case 5: 
      var $12=HEAP32[((88)>>2)]; //@line 595 "main.cpp"
      var $13=(($12+8)|0); //@line 595 "main.cpp"
      var $14=HEAP32[(($13)>>2)]; //@line 595 "main.cpp"
      var $15=HEAP32[((128)>>2)]; //@line 595 "main.cpp"
      var $16=HEAP32[((80)>>2)]; //@line 595 "main.cpp"
      var $17=FUNCTION_TABLE[$14](((88)|0), $15, $16, 5000); //@line 595 "main.cpp"
      if ($17) { label = 7; break; } else { label = 6; break; } //@line 595 "main.cpp"
    case 6: 
      $1=0; //@line 602 "main.cpp"
      label = 7; break; //@line 602 "main.cpp"
    case 7: 
      var $20=$1; //@line 603 "main.cpp"
      STACKTOP = __stackBase__;
      return $20; //@line 603 "main.cpp"
    default: assert(0, "bad label: " + label);
  }
}
Module["_main"] = _main;
function __ZN16loopWrapperClass15executeMainLoopEiim($this, $framesToExecutePerSecond, $maxExecutedFramesWithoutDrawing, $msecToCalculateFPS) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $5;
      var $i;
      var $ended;
      var $timeInit;
      var $timeEnd;
      var $frames;
      var $lastFrameNumber;
      var $frameNumber;
      var $lastFrameTime;
      var $frameTime;
      var $skippedFrames;
      var $framesExecuted;
      $2=$this;
      $3=$framesToExecutePerSecond;
      $4=$maxExecutedFramesWithoutDrawing;
      $5=$msecToCalculateFPS;
      var $6=$2;
      $ended=0; //@line 59 "loopwrap.cpp"
      var $7=$3; //@line 70 "loopwrap.cpp"
      var $8=(($6+32)|0); //@line 70 "loopwrap.cpp"
      HEAP32[(($8)>>2)]=$7; //@line 70 "loopwrap.cpp"
      var $9=(($6+36)|0); //@line 71 "loopwrap.cpp"
      HEAP8[($9)]=0; //@line 71 "loopwrap.cpp"
      var $10=(($6+37)|0); //@line 72 "loopwrap.cpp"
      HEAP8[($10)]=1; //@line 72 "loopwrap.cpp"
      var $11=(($6+8)|0); //@line 73 "loopwrap.cpp"
      var $12=__ZN16timeWrapperClass7isReadyEv($11); //@line 73 "loopwrap.cpp"
      if ($12) { label = 3; break; } else { label = 2; break; } //@line 73 "loopwrap.cpp"
    case 2: 
      var $14=(($6+8)|0); //@line 73 "loopwrap.cpp"
      var $15=__ZN16timeWrapperClass4initEv($14); //@line 73 "loopwrap.cpp"
      label = 3; break; //@line 73 "loopwrap.cpp"
    case 3: 
      var $17=(($6+8)|0); //@line 76 "loopwrap.cpp"
      var $18=__ZN16timeWrapperClass7getTimeEPb($17, 0); //@line 76 "loopwrap.cpp"
      $timeInit=$18; //@line 76 "loopwrap.cpp"
      $frames=0; //@line 79 "loopwrap.cpp"
      $framesExecuted=0; //@line 83 "loopwrap.cpp"
      var $19=$timeInit; //@line 86 "loopwrap.cpp"
      $lastFrameTime=$19; //@line 86 "loopwrap.cpp"
      var $20=$3; //@line 88 "loopwrap.cpp"
      var $21=$lastFrameTime; //@line 88 "loopwrap.cpp"
      var $22=Math.imul($20,$21); //@line 88 "loopwrap.cpp"
      var $23=Math.floor(((($22)>>>(0)))/(1000)); //@line 88 "loopwrap.cpp"
      $lastFrameNumber=$23; //@line 88 "loopwrap.cpp"
      var $24=$6; //@line 91 "loopwrap.cpp"
      var $25=HEAP32[(($24)>>2)]; //@line 91 "loopwrap.cpp"
      var $26=(($25+44)|0); //@line 91 "loopwrap.cpp"
      var $27=HEAP32[(($26)>>2)]; //@line 91 "loopwrap.cpp"
      var $28=FUNCTION_TABLE[$27]($6); //@line 91 "loopwrap.cpp"
      if ($28) { label = 5; break; } else { label = 4; break; } //@line 91 "loopwrap.cpp"
    case 4: 
      $1=0; //@line 91 "loopwrap.cpp"
      label = 29; break; //@line 91 "loopwrap.cpp"
    case 5: 
      label = 6; break; //@line 94 "loopwrap.cpp"
    case 6: 
      var $32=$ended; //@line 94 "loopwrap.cpp"
      var $33=(($32) & 1); //@line 94 "loopwrap.cpp"
      var $34=$33 ^ 1; //@line 94 "loopwrap.cpp"
      if ($34) { label = 7; break; } else { label = 26; break; } //@line 94 "loopwrap.cpp"
    case 7: 
      var $36=$6; //@line 96 "loopwrap.cpp"
      var $37=HEAP32[(($36)>>2)]; //@line 96 "loopwrap.cpp"
      var $38=(($37+40)|0); //@line 96 "loopwrap.cpp"
      var $39=HEAP32[(($38)>>2)]; //@line 96 "loopwrap.cpp"
      var $40=FUNCTION_TABLE[$39]($6); //@line 96 "loopwrap.cpp"
      if ($40) { label = 9; break; } else { label = 8; break; } //@line 96 "loopwrap.cpp"
    case 8: 
      $1=0; //@line 96 "loopwrap.cpp"
      label = 29; break; //@line 96 "loopwrap.cpp"
    case 9: 
      var $43=(($6+8)|0); //@line 99 "loopwrap.cpp"
      var $44=__ZN16timeWrapperClass4playEv($43); //@line 99 "loopwrap.cpp"
      var $45=(($6+8)|0); //@line 103 "loopwrap.cpp"
      var $46=__ZN16timeWrapperClass7getTimeEPb($45, 0); //@line 103 "loopwrap.cpp"
      $frameTime=$46; //@line 103 "loopwrap.cpp"
      var $47=$3; //@line 104 "loopwrap.cpp"
      var $48=$frameTime; //@line 104 "loopwrap.cpp"
      var $49=Math.imul($47,$48); //@line 104 "loopwrap.cpp"
      var $50=Math.floor(((($49)>>>(0)))/(1000)); //@line 104 "loopwrap.cpp"
      $frameNumber=$50; //@line 104 "loopwrap.cpp"
      var $51=$frameNumber; //@line 105 "loopwrap.cpp"
      var $52=$lastFrameNumber; //@line 105 "loopwrap.cpp"
      var $53=((($51)-($52))|0); //@line 105 "loopwrap.cpp"
      $skippedFrames=$53; //@line 105 "loopwrap.cpp"
      var $54=$skippedFrames; //@line 107 "loopwrap.cpp"
      var $55=$4; //@line 107 "loopwrap.cpp"
      var $56=(($54)|(0)) > (($55)|(0)); //@line 107 "loopwrap.cpp"
      if ($56) { label = 10; break; } else { label = 11; break; } //@line 107 "loopwrap.cpp"
    case 10: 
      var $58=$4; //@line 108 "loopwrap.cpp"
      $skippedFrames=$58; //@line 108 "loopwrap.cpp"
      label = 11; break; //@line 108 "loopwrap.cpp"
    case 11: 
      $i=0; //@line 111 "loopwrap.cpp"
      label = 12; break; //@line 111 "loopwrap.cpp"
    case 12: 
      var $61=$i; //@line 111 "loopwrap.cpp"
      var $62=$skippedFrames; //@line 111 "loopwrap.cpp"
      var $63=(($61)|(0)) < (($62)|(0)); //@line 111 "loopwrap.cpp"
      if ($63) { label = 13; break; } else { label = 17; break; } //@line 111 "loopwrap.cpp"
    case 13: 
      var $65=$6; //@line 113 "loopwrap.cpp"
      var $66=HEAP32[(($65)>>2)]; //@line 113 "loopwrap.cpp"
      var $67=(($66+36)|0); //@line 113 "loopwrap.cpp"
      var $68=HEAP32[(($67)>>2)]; //@line 113 "loopwrap.cpp"
      var $69=$framesExecuted; //@line 113 "loopwrap.cpp"
      var $70=FUNCTION_TABLE[$68]($6, $69); //@line 113 "loopwrap.cpp"
      if ($70) { label = 15; break; } else { label = 14; break; } //@line 113 "loopwrap.cpp"
    case 14: 
      $1=0; //@line 113 "loopwrap.cpp"
      label = 29; break; //@line 113 "loopwrap.cpp"
    case 15: 
      var $73=$framesExecuted; //@line 114 "loopwrap.cpp"
      var $74=((($73)+(1))|0); //@line 114 "loopwrap.cpp"
      $framesExecuted=$74; //@line 114 "loopwrap.cpp"
      label = 16; break; //@line 115 "loopwrap.cpp"
    case 16: 
      var $76=$i; //@line 111 "loopwrap.cpp"
      var $77=((($76)+(1))|0); //@line 111 "loopwrap.cpp"
      $i=$77; //@line 111 "loopwrap.cpp"
      label = 12; break; //@line 111 "loopwrap.cpp"
    case 17: 
      var $79=$frameNumber; //@line 116 "loopwrap.cpp"
      $lastFrameNumber=$79; //@line 116 "loopwrap.cpp"
      var $80=$frameTime; //@line 117 "loopwrap.cpp"
      $lastFrameTime=$80; //@line 117 "loopwrap.cpp"
      var $81=$frames; //@line 120 "loopwrap.cpp"
      var $82=((($81)+(1))|0); //@line 120 "loopwrap.cpp"
      $frames=$82; //@line 120 "loopwrap.cpp"
      var $83=$frameTime; //@line 121 "loopwrap.cpp"
      $timeEnd=$83; //@line 121 "loopwrap.cpp"
      var $84=$timeEnd; //@line 122 "loopwrap.cpp"
      var $85=$timeInit; //@line 122 "loopwrap.cpp"
      var $86=((($84)-($85))|0); //@line 122 "loopwrap.cpp"
      var $87=$5; //@line 122 "loopwrap.cpp"
      var $88=(($86)>>>(0)) > (($87)>>>(0)); //@line 122 "loopwrap.cpp"
      if ($88) { label = 18; break; } else { label = 21; break; } //@line 122 "loopwrap.cpp"
    case 18: 
      var $90=$6; //@line 124 "loopwrap.cpp"
      var $91=HEAP32[(($90)>>2)]; //@line 124 "loopwrap.cpp"
      var $92=(($91+52)|0); //@line 124 "loopwrap.cpp"
      var $93=HEAP32[(($92)>>2)]; //@line 124 "loopwrap.cpp"
      var $94=$frames; //@line 124 "loopwrap.cpp"
      var $95=(($94)>>>(0)); //@line 124 "loopwrap.cpp"
      var $96=$timeEnd; //@line 124 "loopwrap.cpp"
      var $97=$timeInit; //@line 124 "loopwrap.cpp"
      var $98=((($96)-($97))|0); //@line 124 "loopwrap.cpp"
      var $99=(($98)>>>(0)); //@line 124 "loopwrap.cpp"
      var $100=($99)/(1000); //@line 124 "loopwrap.cpp"
      var $101=($95)/($100); //@line 124 "loopwrap.cpp"
      var $102=FUNCTION_TABLE[$93]($6, $101); //@line 124 "loopwrap.cpp"
      if ($102) { label = 20; break; } else { label = 19; break; } //@line 124 "loopwrap.cpp"
    case 19: 
      $1=0; //@line 126 "loopwrap.cpp"
      label = 29; break; //@line 126 "loopwrap.cpp"
    case 20: 
      var $105=(($6+8)|0); //@line 129 "loopwrap.cpp"
      var $106=__ZN16timeWrapperClass7getTimeEPb($105, 0); //@line 129 "loopwrap.cpp"
      $timeInit=$106; //@line 129 "loopwrap.cpp"
      $frames=0; //@line 130 "loopwrap.cpp"
      label = 21; break; //@line 131 "loopwrap.cpp"
    case 21: 
      var $108=(($6+37)|0); //@line 143 "loopwrap.cpp"
      var $109=HEAP8[($108)]; //@line 143 "loopwrap.cpp"
      var $110=(($109) & 1); //@line 143 "loopwrap.cpp"
      if ($110) { label = 23; break; } else { label = 22; break; } //@line 143 "loopwrap.cpp"
    case 22: 
      var $112=(($6+8)|0); //@line 143 "loopwrap.cpp"
      var $113=__ZN16timeWrapperClass5pauseEv($112); //@line 143 "loopwrap.cpp"
      label = 23; break; //@line 143 "loopwrap.cpp"
    case 23: 
      var $115=(($6+36)|0); //@line 158 "loopwrap.cpp"
      var $116=HEAP8[($115)]; //@line 158 "loopwrap.cpp"
      var $117=(($116) & 1); //@line 158 "loopwrap.cpp"
      if ($117) { label = 24; break; } else { label = 25; break; } //@line 158 "loopwrap.cpp"
    case 24: 
      $ended=1; //@line 158 "loopwrap.cpp"
      label = 25; break; //@line 158 "loopwrap.cpp"
    case 25: 
      label = 6; break; //@line 159 "loopwrap.cpp"
    case 26: 
      var $121=$6; //@line 162 "loopwrap.cpp"
      var $122=HEAP32[(($121)>>2)]; //@line 162 "loopwrap.cpp"
      var $123=(($122+48)|0); //@line 162 "loopwrap.cpp"
      var $124=HEAP32[(($123)>>2)]; //@line 162 "loopwrap.cpp"
      var $125=FUNCTION_TABLE[$124]($6); //@line 162 "loopwrap.cpp"
      if ($125) { label = 28; break; } else { label = 27; break; } //@line 162 "loopwrap.cpp"
    case 27: 
      $1=0; //@line 162 "loopwrap.cpp"
      label = 29; break; //@line 162 "loopwrap.cpp"
    case 28: 
      $1=1; //@line 165 "loopwrap.cpp"
      label = 29; break; //@line 165 "loopwrap.cpp"
    case 29: 
      var $129=$1; //@line 166 "loopwrap.cpp"
      return $129; //@line 166 "loopwrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN16loopWrapperClassD1Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN16loopWrapperClassD2Ev($2); //@line 60 "./loopwrap.h"
  return; //@line 60 "./loopwrap.h"
}
function __ZNSt9type_infoD2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  return;
}
function __ZNK10__cxxabiv116__shim_type_info5noop1Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  return;
}
function __ZNK10__cxxabiv116__shim_type_info5noop2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  return;
}
function __ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($x, $y, $0) {
  var label = 0;
  var $2;
  var $3;
  var $4;
  $2=$x;
  $3=$y;
  var $5=(($0)&(1));
  $4=$5;
  var $6=$2;
  var $7=$3;
  var $8=(($6)|(0))==(($7)|(0));
  return $8;
}
function __ZNK10__cxxabiv117__class_type_info24process_found_base_classEPNS_19__dynamic_cast_infoEPvi($this, $info, $adjustedPtr, $path_below) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      $1=$this;
      $2=$info;
      $3=$adjustedPtr;
      $4=$path_below;
      var $5=$1;
      var $6=$2;
      var $7=(($6+16)|0);
      var $8=HEAP32[(($7)>>2)];
      var $9=(($8)|(0))==0;
      if ($9) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $11=$3;
      var $12=$2;
      var $13=(($12+16)|0);
      HEAP32[(($13)>>2)]=$11;
      var $14=$4;
      var $15=$2;
      var $16=(($15+24)|0);
      HEAP32[(($16)>>2)]=$14;
      var $17=$2;
      var $18=(($17+36)|0);
      HEAP32[(($18)>>2)]=1;
      label = 9; break;
    case 3: 
      var $20=$2;
      var $21=(($20+16)|0);
      var $22=HEAP32[(($21)>>2)];
      var $23=$3;
      var $24=(($22)|(0))==(($23)|(0));
      if ($24) { label = 4; break; } else { label = 7; break; }
    case 4: 
      var $26=$2;
      var $27=(($26+24)|0);
      var $28=HEAP32[(($27)>>2)];
      var $29=(($28)|(0))==2;
      if ($29) { label = 5; break; } else { label = 6; break; }
    case 5: 
      var $31=$4;
      var $32=$2;
      var $33=(($32+24)|0);
      HEAP32[(($33)>>2)]=$31;
      label = 6; break;
    case 6: 
      label = 8; break;
    case 7: 
      var $36=$2;
      var $37=(($36+36)|0);
      var $38=HEAP32[(($37)>>2)];
      var $39=((($38)+(1))|0);
      HEAP32[(($37)>>2)]=$39;
      var $40=$2;
      var $41=(($40+24)|0);
      HEAP32[(($41)>>2)]=2;
      var $42=$2;
      var $43=(($42+54)|0);
      HEAP8[($43)]=1;
      label = 8; break;
    case 8: 
      label = 9; break;
    case 9: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function __ZNK10__cxxabiv117__class_type_info29process_static_type_above_dstEPNS_19__dynamic_cast_infoEPKvS4_i($this, $info, $dst_ptr, $current_ptr, $path_below) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $5;
      $1=$this;
      $2=$info;
      $3=$dst_ptr;
      $4=$current_ptr;
      $5=$path_below;
      var $6=$1;
      var $7=$2;
      var $8=(($7+53)|0);
      HEAP8[($8)]=1;
      var $9=$4;
      var $10=$2;
      var $11=(($10+4)|0);
      var $12=HEAP32[(($11)>>2)];
      var $13=(($9)|(0))==(($12)|(0));
      if ($13) { label = 2; break; } else { label = 17; break; }
    case 2: 
      var $15=$2;
      var $16=(($15+52)|0);
      HEAP8[($16)]=1;
      var $17=$2;
      var $18=(($17+16)|0);
      var $19=HEAP32[(($18)>>2)];
      var $20=(($19)|(0))==0;
      if ($20) { label = 3; break; } else { label = 7; break; }
    case 3: 
      var $22=$3;
      var $23=$2;
      var $24=(($23+16)|0);
      HEAP32[(($24)>>2)]=$22;
      var $25=$5;
      var $26=$2;
      var $27=(($26+24)|0);
      HEAP32[(($27)>>2)]=$25;
      var $28=$2;
      var $29=(($28+36)|0);
      HEAP32[(($29)>>2)]=1;
      var $30=$2;
      var $31=(($30+48)|0);
      var $32=HEAP32[(($31)>>2)];
      var $33=(($32)|(0))==1;
      if ($33) { label = 4; break; } else { label = 6; break; }
    case 4: 
      var $35=$2;
      var $36=(($35+24)|0);
      var $37=HEAP32[(($36)>>2)];
      var $38=(($37)|(0))==1;
      if ($38) { label = 5; break; } else { label = 6; break; }
    case 5: 
      var $40=$2;
      var $41=(($40+54)|0);
      HEAP8[($41)]=1;
      label = 6; break;
    case 6: 
      label = 16; break;
    case 7: 
      var $44=$2;
      var $45=(($44+16)|0);
      var $46=HEAP32[(($45)>>2)];
      var $47=$3;
      var $48=(($46)|(0))==(($47)|(0));
      if ($48) { label = 8; break; } else { label = 14; break; }
    case 8: 
      var $50=$2;
      var $51=(($50+24)|0);
      var $52=HEAP32[(($51)>>2)];
      var $53=(($52)|(0))==2;
      if ($53) { label = 9; break; } else { label = 10; break; }
    case 9: 
      var $55=$5;
      var $56=$2;
      var $57=(($56+24)|0);
      HEAP32[(($57)>>2)]=$55;
      label = 10; break;
    case 10: 
      var $59=$2;
      var $60=(($59+48)|0);
      var $61=HEAP32[(($60)>>2)];
      var $62=(($61)|(0))==1;
      if ($62) { label = 11; break; } else { label = 13; break; }
    case 11: 
      var $64=$2;
      var $65=(($64+24)|0);
      var $66=HEAP32[(($65)>>2)];
      var $67=(($66)|(0))==1;
      if ($67) { label = 12; break; } else { label = 13; break; }
    case 12: 
      var $69=$2;
      var $70=(($69+54)|0);
      HEAP8[($70)]=1;
      label = 13; break;
    case 13: 
      label = 15; break;
    case 14: 
      var $73=$2;
      var $74=(($73+36)|0);
      var $75=HEAP32[(($74)>>2)];
      var $76=((($75)+(1))|0);
      HEAP32[(($74)>>2)]=$76;
      var $77=$2;
      var $78=(($77+54)|0);
      HEAP8[($78)]=1;
      label = 15; break;
    case 15: 
      label = 16; break;
    case 16: 
      label = 17; break;
    case 17: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function __ZNK10__cxxabiv117__class_type_info29process_static_type_below_dstEPNS_19__dynamic_cast_infoEPKvi($this, $info, $current_ptr, $path_below) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      $1=$this;
      $2=$info;
      $3=$current_ptr;
      $4=$path_below;
      var $5=$1;
      var $6=$3;
      var $7=$2;
      var $8=(($7+4)|0);
      var $9=HEAP32[(($8)>>2)];
      var $10=(($6)|(0))==(($9)|(0));
      if ($10) { label = 2; break; } else { label = 5; break; }
    case 2: 
      var $12=$2;
      var $13=(($12+28)|0);
      var $14=HEAP32[(($13)>>2)];
      var $15=(($14)|(0))!=1;
      if ($15) { label = 3; break; } else { label = 4; break; }
    case 3: 
      var $17=$4;
      var $18=$2;
      var $19=(($18+28)|0);
      HEAP32[(($19)>>2)]=$17;
      label = 4; break;
    case 4: 
      label = 5; break;
    case 5: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function __ZN16loopWrapperClassD0Ev($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      $1=$this;
      var $4=$1;
      (function() { try { __THREW__ = 0; return __ZN16loopWrapperClassD1Ev($4) } catch(e) { if (typeof e != "number") throw e; if (ABORT) throw e; __THREW__ = 1; return null } })();if (!__THREW__) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $6=$4; //@line 60 "./loopwrap.h"
      __ZdlPv($6); //@line 60 "./loopwrap.h"
      return; //@line 60 "./loopwrap.h"
    case 3: 
      var $8$0 = ___cxa_find_matching_catch(-1, -1); $8$1 = tempRet0;
      var $9=$8$0;
      $2=$9; //@line 60 "./loopwrap.h"
      var $10=$8$1;
      $3=$10; //@line 60 "./loopwrap.h"
      var $11=$4; //@line 60 "./loopwrap.h"
      __ZdlPv($11); //@line 60 "./loopwrap.h"
      label = 4; break; //@line 60 "./loopwrap.h"
    case 4: 
      var $13=$2; //@line 60 "./loopwrap.h"
      var $14=$3; //@line 60 "./loopwrap.h"
      var $15$0=$13;
      var $15$1=0;
      var $16$0=$15$0;
      var $16$1=$14;
      ___resumeException($16$0) //@line 60 "./loopwrap.h"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN16timeWrapperClass4initEv($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      $2=$this;
      var $3=$2;
      var $4=__ZN16timeWrapperClass7isReadyEv($3); //@line 35 "timewrap.cpp"
      if ($4) { label = 2; break; } else { label = 3; break; } //@line 35 "timewrap.cpp"
    case 2: 
      $1=0; //@line 35 "timewrap.cpp"
      label = 6; break; //@line 35 "timewrap.cpp"
    case 3: 
      var $7=(($3+4)|0); //@line 38 "timewrap.cpp"
      HEAP8[($7)]=1; //@line 38 "timewrap.cpp"
      var $8=(($3+5)|0); //@line 39 "timewrap.cpp"
      HEAP8[($8)]=0; //@line 39 "timewrap.cpp"
      var $9=(($3+16)|0); //@line 40 "timewrap.cpp"
      HEAPF64[(($9)>>3)]=1; //@line 40 "timewrap.cpp"
      var $10=__ZN16timeWrapperClass5resetEv($3); //@line 43 "timewrap.cpp"
      if ($10) { label = 5; break; } else { label = 4; break; } //@line 43 "timewrap.cpp"
    case 4: 
      var $12=(($3+4)|0); //@line 45 "timewrap.cpp"
      HEAP8[($12)]=0; //@line 45 "timewrap.cpp"
      $1=0; //@line 46 "timewrap.cpp"
      label = 6; break; //@line 46 "timewrap.cpp"
    case 5: 
      $1=1; //@line 49 "timewrap.cpp"
      label = 6; break; //@line 49 "timewrap.cpp"
    case 6: 
      var $15=$1; //@line 50 "timewrap.cpp"
      return $15; //@line 50 "timewrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN16timeWrapperClass5resetEv($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      $2=$this;
      var $3=$2;
      var $4=__ZN16timeWrapperClass7isReadyEv($3); //@line 69 "timewrap.cpp"
      if ($4) { label = 3; break; } else { label = 2; break; } //@line 69 "timewrap.cpp"
    case 2: 
      $1=0; //@line 69 "timewrap.cpp"
      label = 4; break; //@line 69 "timewrap.cpp"
    case 3: 
      var $7=_SDL_GetTicks(); //@line 72 "timewrap.cpp"
      var $8=(($3+8)|0); //@line 72 "timewrap.cpp"
      HEAP32[(($8)>>2)]=$7; //@line 72 "timewrap.cpp"
      var $9=(($3+12)|0); //@line 73 "timewrap.cpp"
      HEAP32[(($9)>>2)]=0; //@line 73 "timewrap.cpp"
      var $10=(($3+16)|0); //@line 74 "timewrap.cpp"
      HEAPF64[(($10)>>3)]=1; //@line 74 "timewrap.cpp"
      $1=1; //@line 76 "timewrap.cpp"
      label = 4; break; //@line 76 "timewrap.cpp"
    case 4: 
      var $12=$1; //@line 77 "timewrap.cpp"
      return $12; //@line 77 "timewrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN16timeWrapperClass3endEv($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      $2=$this;
      var $3=$2;
      var $4=__ZN16timeWrapperClass7isReadyEv($3); //@line 56 "timewrap.cpp"
      if ($4) { label = 3; break; } else { label = 2; break; } //@line 56 "timewrap.cpp"
    case 2: 
      $1=0; //@line 56 "timewrap.cpp"
      label = 4; break; //@line 56 "timewrap.cpp"
    case 3: 
      var $7=(($3+4)|0); //@line 59 "timewrap.cpp"
      HEAP8[($7)]=0; //@line 59 "timewrap.cpp"
      $1=1; //@line 61 "timewrap.cpp"
      label = 4; break; //@line 61 "timewrap.cpp"
    case 4: 
      var $9=$1; //@line 62 "timewrap.cpp"
      return $9; //@line 62 "timewrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN16timeWrapperClass5pauseEv($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      $2=$this;
      var $3=$2;
      var $4=__ZN16timeWrapperClass7isReadyEv($3); //@line 83 "timewrap.cpp"
      if ($4) { label = 3; break; } else { label = 2; break; } //@line 83 "timewrap.cpp"
    case 2: 
      $1=0; //@line 83 "timewrap.cpp"
      label = 6; break; //@line 83 "timewrap.cpp"
    case 3: 
      var $7=(($3+5)|0); //@line 84 "timewrap.cpp"
      var $8=HEAP8[($7)]; //@line 84 "timewrap.cpp"
      var $9=(($8) & 1); //@line 84 "timewrap.cpp"
      if ($9) { label = 4; break; } else { label = 5; break; } //@line 84 "timewrap.cpp"
    case 4: 
      $1=1; //@line 84 "timewrap.cpp"
      label = 6; break; //@line 84 "timewrap.cpp"
    case 5: 
      var $12=__ZN16timeWrapperClass7getTimeEPb($3, 0); //@line 88 "timewrap.cpp"
      var $13=(($3+12)|0); //@line 88 "timewrap.cpp"
      HEAP32[(($13)>>2)]=$12; //@line 88 "timewrap.cpp"
      var $14=(($3+5)|0); //@line 91 "timewrap.cpp"
      HEAP8[($14)]=1; //@line 91 "timewrap.cpp"
      $1=1; //@line 93 "timewrap.cpp"
      label = 6; break; //@line 93 "timewrap.cpp"
    case 6: 
      var $16=$1; //@line 94 "timewrap.cpp"
      return $16; //@line 94 "timewrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN16timeWrapperClass7getTimeEPb($this, $errorCode) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $tickCount;
      var $tempTime;
      $2=$this;
      $3=$errorCode;
      var $4=$2;
      var $5=__ZN16timeWrapperClass7isReadyEv($4); //@line 139 "timewrap.cpp"
      if ($5) { label = 5; break; } else { label = 2; break; } //@line 139 "timewrap.cpp"
    case 2: 
      var $7=$3; //@line 141 "timewrap.cpp"
      var $8=(($7)|(0))!=0; //@line 141 "timewrap.cpp"
      if ($8) { label = 3; break; } else { label = 4; break; } //@line 141 "timewrap.cpp"
    case 3: 
      var $10=$3; //@line 141 "timewrap.cpp"
      HEAP8[($10)]=0; //@line 141 "timewrap.cpp"
      label = 4; break; //@line 141 "timewrap.cpp"
    case 4: 
      $1=0; //@line 142 "timewrap.cpp"
      label = 13; break; //@line 142 "timewrap.cpp"
    case 5: 
      var $13=_SDL_GetTicks(); //@line 146 "timewrap.cpp"
      $tickCount=$13; //@line 146 "timewrap.cpp"
      var $14=$tickCount; //@line 147 "timewrap.cpp"
      var $15=(($4+8)|0); //@line 147 "timewrap.cpp"
      var $16=HEAP32[(($15)>>2)]; //@line 147 "timewrap.cpp"
      var $17=(($14)>>>(0)) < (($16)>>>(0)); //@line 147 "timewrap.cpp"
      if ($17) { label = 6; break; } else { label = 7; break; } //@line 147 "timewrap.cpp"
    case 6: 
      var $19=(($4+8)|0); //@line 152 "timewrap.cpp"
      var $20=HEAP32[(($19)>>2)]; //@line 152 "timewrap.cpp"
      var $21$0=$20;
      var $21$1=0;
      var $$etemp$0$0=-1;
      var $$etemp$0$1=0;
      var $22$0 = _i64Subtract($$etemp$0$0,$$etemp$0$1,$21$0,$21$1); var $22$1 = tempRet0; //@line 152 "timewrap.cpp"
      var $23=(($4+12)|0); //@line 152 "timewrap.cpp"
      var $24=HEAP32[(($23)>>2)]; //@line 152 "timewrap.cpp"
      var $25$0=$24;
      var $25$1=0;
      var $26$0 = _i64Subtract($22$0,$22$1,$25$0,$25$1); var $26$1 = tempRet0; //@line 152 "timewrap.cpp"
      var $$etemp$1$0=1;
      var $$etemp$1$1=0;
      var $27$0 = _i64Add($26$0,$26$1,$$etemp$1$0,$$etemp$1$1); var $27$1 = tempRet0; //@line 152 "timewrap.cpp"
      var $28=((($27$0)>>>(0))+((($27$1)|(0))*4294967296)); //@line 152 "timewrap.cpp"
      var $29=(($4+16)|0); //@line 152 "timewrap.cpp"
      var $30=HEAPF64[(($29)>>3)]; //@line 152 "timewrap.cpp"
      var $31=($28)*($30); //@line 152 "timewrap.cpp"
      var $32=($31>=0 ? Math.floor($31) : Math.ceil($31)); //@line 152 "timewrap.cpp"
      var $33=(($4+12)|0); //@line 152 "timewrap.cpp"
      var $34=HEAP32[(($33)>>2)]; //@line 152 "timewrap.cpp"
      var $35=((($32)+($34))|0); //@line 152 "timewrap.cpp"
      var $36=(($4+12)|0); //@line 152 "timewrap.cpp"
      HEAP32[(($36)>>2)]=$35; //@line 152 "timewrap.cpp"
      var $37=(($4+8)|0); //@line 154 "timewrap.cpp"
      HEAP32[(($37)>>2)]=0; //@line 154 "timewrap.cpp"
      label = 7; break; //@line 155 "timewrap.cpp"
    case 7: 
      var $39=(($4+5)|0); //@line 157 "timewrap.cpp"
      var $40=HEAP8[($39)]; //@line 157 "timewrap.cpp"
      var $41=(($40) & 1); //@line 157 "timewrap.cpp"
      if ($41) { label = 8; break; } else { label = 9; break; } //@line 157 "timewrap.cpp"
    case 8: 
      var $43=(($4+12)|0); //@line 159 "timewrap.cpp"
      var $44=HEAP32[(($43)>>2)]; //@line 159 "timewrap.cpp"
      $tempTime=$44; //@line 159 "timewrap.cpp"
      label = 10; break; //@line 160 "timewrap.cpp"
    case 9: 
      var $46=$tickCount; //@line 168 "timewrap.cpp"
      var $47=(($4+8)|0); //@line 168 "timewrap.cpp"
      var $48=HEAP32[(($47)>>2)]; //@line 168 "timewrap.cpp"
      var $49=((($46)-($48))|0); //@line 168 "timewrap.cpp"
      var $50=(($49)>>>(0)); //@line 168 "timewrap.cpp"
      var $51=(($4+16)|0); //@line 168 "timewrap.cpp"
      var $52=HEAPF64[(($51)>>3)]; //@line 168 "timewrap.cpp"
      var $53=($50)*($52); //@line 168 "timewrap.cpp"
      var $54=($53>=0 ? Math.floor($53) : Math.ceil($53)); //@line 168 "timewrap.cpp"
      var $55=(($4+12)|0); //@line 168 "timewrap.cpp"
      var $56=HEAP32[(($55)>>2)]; //@line 168 "timewrap.cpp"
      var $57=((($54)+($56))|0); //@line 168 "timewrap.cpp"
      $tempTime=$57; //@line 168 "timewrap.cpp"
      label = 10; break;
    case 10: 
      var $59=$3; //@line 172 "timewrap.cpp"
      var $60=(($59)|(0))!=0; //@line 172 "timewrap.cpp"
      if ($60) { label = 11; break; } else { label = 12; break; } //@line 172 "timewrap.cpp"
    case 11: 
      var $62=$3; //@line 172 "timewrap.cpp"
      HEAP8[($62)]=1; //@line 172 "timewrap.cpp"
      label = 12; break; //@line 172 "timewrap.cpp"
    case 12: 
      var $64=$tempTime; //@line 174 "timewrap.cpp"
      $1=$64; //@line 174 "timewrap.cpp"
      label = 13; break; //@line 174 "timewrap.cpp"
    case 13: 
      var $66=$1; //@line 175 "timewrap.cpp"
      return $66; //@line 175 "timewrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN16timeWrapperClass4playEv($this) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      $2=$this;
      var $3=$2;
      var $4=__ZN16timeWrapperClass7isReadyEv($3); //@line 100 "timewrap.cpp"
      if ($4) { label = 3; break; } else { label = 2; break; } //@line 100 "timewrap.cpp"
    case 2: 
      $1=0; //@line 100 "timewrap.cpp"
      label = 6; break; //@line 100 "timewrap.cpp"
    case 3: 
      var $7=(($3+5)|0); //@line 101 "timewrap.cpp"
      var $8=HEAP8[($7)]; //@line 101 "timewrap.cpp"
      var $9=(($8) & 1); //@line 101 "timewrap.cpp"
      if ($9) { label = 5; break; } else { label = 4; break; } //@line 101 "timewrap.cpp"
    case 4: 
      $1=1; //@line 101 "timewrap.cpp"
      label = 6; break; //@line 101 "timewrap.cpp"
    case 5: 
      var $12=_SDL_GetTicks(); //@line 106 "timewrap.cpp"
      var $13=(($3+8)|0); //@line 106 "timewrap.cpp"
      HEAP32[(($13)>>2)]=$12; //@line 106 "timewrap.cpp"
      var $14=(($3+5)|0); //@line 109 "timewrap.cpp"
      HEAP8[($14)]=0; //@line 109 "timewrap.cpp"
      $1=1; //@line 111 "timewrap.cpp"
      label = 6; break; //@line 111 "timewrap.cpp"
    case 6: 
      var $16=$1; //@line 112 "timewrap.cpp"
      return $16; //@line 112 "timewrap.cpp"
    default: assert(0, "bad label: " + label);
  }
}
function __ZN10__cxxabiv116__shim_type_infoD2Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  var $3=$2;
  __ZNSt9type_infoD2Ev($3);
  return;
}
function __ZN10__cxxabiv117__class_type_infoD0Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN10__cxxabiv116__shim_type_infoD2Ev($2);
  var $3=$2;
  __ZdlPv($3);
  return;
}
function __ZN10__cxxabiv120__si_class_type_infoD0Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN10__cxxabiv116__shim_type_infoD2Ev($2);
  var $3=$2;
  __ZdlPv($3);
  return;
}
function __ZNK10__cxxabiv117__class_type_info9can_catchEPKNS_16__shim_type_infoERPv($this, $thrown_type, $adjustedPtr) {
  var label = 0;
  var __stackBase__  = STACKTOP; STACKTOP = (STACKTOP + 56)|0; assert((STACKTOP|0) < (STACK_MAX|0));
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $thrown_class_type;
      var $info=__stackBase__;
      $2=$this;
      $3=$thrown_type;
      $4=$adjustedPtr;
      var $5=$2;
      var $6=$5;
      var $7=$3;
      var $8=$7;
      var $9=__ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($6, $8, 0);
      if ($9) { label = 2; break; } else { label = 3; break; }
    case 2: 
      $1=1;
      label = 11; break;
    case 3: 
      var $12=$3;
      var $13=(($12)|(0))==0;
      if ($13) { label = 5; break; } else { label = 4; break; }
    case 4: 
      var $15=$12;
      var $16=___dynamic_cast($15, 1592, 1576, -1);
      var $17=$16;
      var $20 = $17;label = 6; break;
    case 5: 
      var $20 = 0;label = 6; break;
    case 6: 
      var $20;
      $thrown_class_type=$20;
      var $21=$thrown_class_type;
      var $22=(($21)|(0))==0;
      if ($22) { label = 7; break; } else { label = 8; break; }
    case 7: 
      $1=0;
      label = 11; break;
    case 8: 
      var $25=$info;
      _memset($25, 0, 56);
      var $26=(($info)|0);
      var $27=$thrown_class_type;
      HEAP32[(($26)>>2)]=$27;
      var $28=(($info+8)|0);
      HEAP32[(($28)>>2)]=$5;
      var $29=(($info+12)|0);
      HEAP32[(($29)>>2)]=-1;
      var $30=(($info+48)|0);
      HEAP32[(($30)>>2)]=1;
      var $31=$thrown_class_type;
      var $32=$31;
      var $33=HEAP32[(($32)>>2)];
      var $34=(($33+28)|0);
      var $35=HEAP32[(($34)>>2)];
      var $36=$4;
      var $37=HEAP32[(($36)>>2)];
      FUNCTION_TABLE[$35]($31, $info, $37, 1);
      var $38=(($info+24)|0);
      var $39=HEAP32[(($38)>>2)];
      var $40=(($39)|(0))==1;
      if ($40) { label = 9; break; } else { label = 10; break; }
    case 9: 
      var $42=(($info+16)|0);
      var $43=HEAP32[(($42)>>2)];
      var $44=$4;
      HEAP32[(($44)>>2)]=$43;
      $1=1;
      label = 11; break;
    case 10: 
      $1=0;
      label = 11; break;
    case 11: 
      var $47=$1;
      STACKTOP = __stackBase__;
      return $47;
    default: assert(0, "bad label: " + label);
  }
}
function __ZNK10__cxxabiv117__class_type_info27has_unambiguous_public_baseEPNS_19__dynamic_cast_infoEPvi($this, $info, $adjustedPtr, $path_below) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      $1=$this;
      $2=$info;
      $3=$adjustedPtr;
      $4=$path_below;
      var $5=$1;
      var $6=$5;
      var $7=$2;
      var $8=(($7+8)|0);
      var $9=HEAP32[(($8)>>2)];
      var $10=$9;
      var $11=__ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($6, $10, 0);
      if ($11) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $13=$2;
      var $14=$3;
      var $15=$4;
      __ZNK10__cxxabiv117__class_type_info24process_found_base_classEPNS_19__dynamic_cast_infoEPvi($5, $13, $14, $15);
      label = 3; break;
    case 3: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function __ZNK10__cxxabiv120__si_class_type_info27has_unambiguous_public_baseEPNS_19__dynamic_cast_infoEPvi($this, $info, $adjustedPtr, $path_below) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      $1=$this;
      $2=$info;
      $3=$adjustedPtr;
      $4=$path_below;
      var $5=$1;
      var $6=$5;
      var $7=$2;
      var $8=(($7+8)|0);
      var $9=HEAP32[(($8)>>2)];
      var $10=$9;
      var $11=__ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($6, $10, 0);
      if ($11) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $13=$5;
      var $14=$2;
      var $15=$3;
      var $16=$4;
      __ZNK10__cxxabiv117__class_type_info24process_found_base_classEPNS_19__dynamic_cast_infoEPvi($13, $14, $15, $16);
      label = 4; break;
    case 3: 
      var $18=(($5+8)|0);
      var $19=HEAP32[(($18)>>2)];
      var $20=$19;
      var $21=HEAP32[(($20)>>2)];
      var $22=(($21+28)|0);
      var $23=HEAP32[(($22)>>2)];
      var $24=$2;
      var $25=$3;
      var $26=$4;
      FUNCTION_TABLE[$23]($19, $24, $25, $26);
      label = 4; break;
    case 4: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function ___dynamic_cast($static_ptr, $static_type, $dst_type, $src2dst_offset) {
  var label = 0;
  var __stackBase__  = STACKTOP; STACKTOP = (STACKTOP + 56)|0; assert((STACKTOP|0) < (STACK_MAX|0));
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $vtable;
      var $offset_to_derived;
      var $dynamic_ptr;
      var $dynamic_type;
      var $dst_ptr;
      var $info=__stackBase__;
      $1=$static_ptr;
      $2=$static_type;
      $3=$dst_type;
      $4=$src2dst_offset;
      var $5=$1;
      var $6=$5;
      var $7=HEAP32[(($6)>>2)];
      $vtable=$7;
      var $8=$vtable;
      var $9=((($8)-(8))|0);
      var $10=HEAP32[(($9)>>2)];
      var $11=$10;
      $offset_to_derived=$11;
      var $12=$1;
      var $13=$offset_to_derived;
      var $14=(($12+$13)|0);
      $dynamic_ptr=$14;
      var $15=$vtable;
      var $16=((($15)-(4))|0);
      var $17=HEAP32[(($16)>>2)];
      var $18=$17;
      $dynamic_type=$18;
      $dst_ptr=0;
      var $19=(($info)|0);
      var $20=$3;
      HEAP32[(($19)>>2)]=$20;
      var $21=(($info+4)|0);
      var $22=$1;
      HEAP32[(($21)>>2)]=$22;
      var $23=(($info+8)|0);
      var $24=$2;
      HEAP32[(($23)>>2)]=$24;
      var $25=(($info+12)|0);
      var $26=$4;
      HEAP32[(($25)>>2)]=$26;
      var $27=(($info+16)|0);
      HEAP32[(($27)>>2)]=0;
      var $28=(($info+20)|0);
      HEAP32[(($28)>>2)]=0;
      var $29=(($info+24)|0);
      HEAP32[(($29)>>2)]=0;
      var $30=(($info+28)|0);
      HEAP32[(($30)>>2)]=0;
      var $31=(($info+32)|0);
      HEAP32[(($31)>>2)]=0;
      var $32=(($info+36)|0);
      HEAP32[(($32)>>2)]=0;
      var $33=(($info+40)|0);
      HEAP32[(($33)>>2)]=0;
      var $34=(($info+44)|0);
      HEAP32[(($34)>>2)]=0;
      var $35=(($info+48)|0);
      HEAP32[(($35)>>2)]=0;
      var $36=(($info+52)|0);
      HEAP8[($36)]=0;
      var $37=(($info+53)|0);
      HEAP8[($37)]=0;
      var $38=(($info+54)|0);
      HEAP8[($38)]=0;
      var $39=$dynamic_type;
      var $40=$39;
      var $41=$3;
      var $42=$41;
      var $43=__ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($40, $42, 0);
      if ($43) { label = 2; break; } else { label = 5; break; }
    case 2: 
      var $45=(($info+48)|0);
      HEAP32[(($45)>>2)]=1;
      var $46=$dynamic_type;
      var $47=$46;
      var $48=HEAP32[(($47)>>2)];
      var $49=(($48+20)|0);
      var $50=HEAP32[(($49)>>2)];
      var $51=$dynamic_ptr;
      var $52=$dynamic_ptr;
      FUNCTION_TABLE[$50]($46, $info, $51, $52, 1, 0);
      var $53=(($info+24)|0);
      var $54=HEAP32[(($53)>>2)];
      var $55=(($54)|(0))==1;
      if ($55) { label = 3; break; } else { label = 4; break; }
    case 3: 
      var $57=$dynamic_ptr;
      $dst_ptr=$57;
      label = 4; break;
    case 4: 
      label = 18; break;
    case 5: 
      var $60=$dynamic_type;
      var $61=$60;
      var $62=HEAP32[(($61)>>2)];
      var $63=(($62+24)|0);
      var $64=HEAP32[(($63)>>2)];
      var $65=$dynamic_ptr;
      FUNCTION_TABLE[$64]($60, $info, $65, 1, 0);
      var $66=(($info+36)|0);
      var $67=HEAP32[(($66)>>2)];
      if ((($67)|(0))==0) {
        label = 6; break;
      }
      else if ((($67)|(0))==1) {
        label = 11; break;
      }
      else {
      label = 17; break;
      }
    case 6: 
      var $69=(($info+40)|0);
      var $70=HEAP32[(($69)>>2)];
      var $71=(($70)|(0))==1;
      if ($71) { label = 7; break; } else { label = 10; break; }
    case 7: 
      var $73=(($info+28)|0);
      var $74=HEAP32[(($73)>>2)];
      var $75=(($74)|(0))==1;
      if ($75) { label = 8; break; } else { label = 10; break; }
    case 8: 
      var $77=(($info+32)|0);
      var $78=HEAP32[(($77)>>2)];
      var $79=(($78)|(0))==1;
      if ($79) { label = 9; break; } else { label = 10; break; }
    case 9: 
      var $81=(($info+20)|0);
      var $82=HEAP32[(($81)>>2)];
      $dst_ptr=$82;
      label = 10; break;
    case 10: 
      label = 17; break;
    case 11: 
      var $85=(($info+24)|0);
      var $86=HEAP32[(($85)>>2)];
      var $87=(($86)|(0))==1;
      if ($87) { label = 15; break; } else { label = 12; break; }
    case 12: 
      var $89=(($info+40)|0);
      var $90=HEAP32[(($89)>>2)];
      var $91=(($90)|(0))==0;
      if ($91) { label = 13; break; } else { label = 16; break; }
    case 13: 
      var $93=(($info+28)|0);
      var $94=HEAP32[(($93)>>2)];
      var $95=(($94)|(0))==1;
      if ($95) { label = 14; break; } else { label = 16; break; }
    case 14: 
      var $97=(($info+32)|0);
      var $98=HEAP32[(($97)>>2)];
      var $99=(($98)|(0))==1;
      if ($99) { label = 15; break; } else { label = 16; break; }
    case 15: 
      var $101=(($info+16)|0);
      var $102=HEAP32[(($101)>>2)];
      $dst_ptr=$102;
      label = 16; break;
    case 16: 
      label = 17; break;
    case 17: 
      label = 18; break;
    case 18: 
      var $106=$dst_ptr;
      STACKTOP = __stackBase__;
      return $106;
    default: assert(0, "bad label: " + label);
  }
}
function __ZNK10__cxxabiv120__si_class_type_info16search_below_dstEPNS_19__dynamic_cast_infoEPKvib($this, $info, $current_ptr, $path_below, $use_strcmp) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $5;
      var $is_dst_type_derived_from_static_type;
      var $does_dst_type_point_to_our_static_type;
      $1=$this;
      $2=$info;
      $3=$current_ptr;
      $4=$path_below;
      var $6=(($use_strcmp)&(1));
      $5=$6;
      var $7=$1;
      var $8=$7;
      var $9=$2;
      var $10=(($9+8)|0);
      var $11=HEAP32[(($10)>>2)];
      var $12=$11;
      var $13=$5;
      var $14=(($13) & 1);
      var $15=__ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($8, $12, $14);
      if ($15) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $17=$7;
      var $18=$2;
      var $19=$3;
      var $20=$4;
      __ZNK10__cxxabiv117__class_type_info29process_static_type_below_dstEPNS_19__dynamic_cast_infoEPKvi($17, $18, $19, $20);
      label = 27; break;
    case 3: 
      var $22=$7;
      var $23=$2;
      var $24=(($23)|0);
      var $25=HEAP32[(($24)>>2)];
      var $26=$25;
      var $27=$5;
      var $28=(($27) & 1);
      var $29=__ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($22, $26, $28);
      if ($29) { label = 4; break; } else { label = 25; break; }
    case 4: 
      var $31=$3;
      var $32=$2;
      var $33=(($32+16)|0);
      var $34=HEAP32[(($33)>>2)];
      var $35=(($31)|(0))==(($34)|(0));
      if ($35) { label = 6; break; } else { label = 5; break; }
    case 5: 
      var $37=$3;
      var $38=$2;
      var $39=(($38+20)|0);
      var $40=HEAP32[(($39)>>2)];
      var $41=(($37)|(0))==(($40)|(0));
      if ($41) { label = 6; break; } else { label = 9; break; }
    case 6: 
      var $43=$4;
      var $44=(($43)|(0))==1;
      if ($44) { label = 7; break; } else { label = 8; break; }
    case 7: 
      var $46=$2;
      var $47=(($46+32)|0);
      HEAP32[(($47)>>2)]=1;
      label = 8; break;
    case 8: 
      label = 24; break;
    case 9: 
      var $50=$4;
      var $51=$2;
      var $52=(($51+32)|0);
      HEAP32[(($52)>>2)]=$50;
      var $53=$2;
      var $54=(($53+44)|0);
      var $55=HEAP32[(($54)>>2)];
      var $56=(($55)|(0))!=4;
      if ($56) { label = 10; break; } else { label = 23; break; }
    case 10: 
      $is_dst_type_derived_from_static_type=0;
      $does_dst_type_point_to_our_static_type=0;
      var $58=$2;
      var $59=(($58+52)|0);
      HEAP8[($59)]=0;
      var $60=$2;
      var $61=(($60+53)|0);
      HEAP8[($61)]=0;
      var $62=(($7+8)|0);
      var $63=HEAP32[(($62)>>2)];
      var $64=$63;
      var $65=HEAP32[(($64)>>2)];
      var $66=(($65+20)|0);
      var $67=HEAP32[(($66)>>2)];
      var $68=$2;
      var $69=$3;
      var $70=$3;
      var $71=$5;
      var $72=(($71) & 1);
      FUNCTION_TABLE[$67]($63, $68, $69, $70, 1, $72);
      var $73=$2;
      var $74=(($73+53)|0);
      var $75=HEAP8[($74)];
      var $76=(($75) & 1);
      if ($76) { label = 11; break; } else { label = 14; break; }
    case 11: 
      $is_dst_type_derived_from_static_type=1;
      var $78=$2;
      var $79=(($78+52)|0);
      var $80=HEAP8[($79)];
      var $81=(($80) & 1);
      if ($81) { label = 12; break; } else { label = 13; break; }
    case 12: 
      $does_dst_type_point_to_our_static_type=1;
      label = 13; break;
    case 13: 
      label = 14; break;
    case 14: 
      var $85=$does_dst_type_point_to_our_static_type;
      var $86=(($85) & 1);
      if ($86) { label = 19; break; } else { label = 15; break; }
    case 15: 
      var $88=$3;
      var $89=$2;
      var $90=(($89+20)|0);
      HEAP32[(($90)>>2)]=$88;
      var $91=$2;
      var $92=(($91+40)|0);
      var $93=HEAP32[(($92)>>2)];
      var $94=((($93)+(1))|0);
      HEAP32[(($92)>>2)]=$94;
      var $95=$2;
      var $96=(($95+36)|0);
      var $97=HEAP32[(($96)>>2)];
      var $98=(($97)|(0))==1;
      if ($98) { label = 16; break; } else { label = 18; break; }
    case 16: 
      var $100=$2;
      var $101=(($100+24)|0);
      var $102=HEAP32[(($101)>>2)];
      var $103=(($102)|(0))==2;
      if ($103) { label = 17; break; } else { label = 18; break; }
    case 17: 
      var $105=$2;
      var $106=(($105+54)|0);
      HEAP8[($106)]=1;
      label = 18; break;
    case 18: 
      label = 19; break;
    case 19: 
      var $109=$is_dst_type_derived_from_static_type;
      var $110=(($109) & 1);
      if ($110) { label = 20; break; } else { label = 21; break; }
    case 20: 
      var $112=$2;
      var $113=(($112+44)|0);
      HEAP32[(($113)>>2)]=3;
      label = 22; break;
    case 21: 
      var $115=$2;
      var $116=(($115+44)|0);
      HEAP32[(($116)>>2)]=4;
      label = 22; break;
    case 22: 
      label = 23; break;
    case 23: 
      label = 24; break;
    case 24: 
      label = 26; break;
    case 25: 
      var $121=(($7+8)|0);
      var $122=HEAP32[(($121)>>2)];
      var $123=$122;
      var $124=HEAP32[(($123)>>2)];
      var $125=(($124+24)|0);
      var $126=HEAP32[(($125)>>2)];
      var $127=$2;
      var $128=$3;
      var $129=$4;
      var $130=$5;
      var $131=(($130) & 1);
      FUNCTION_TABLE[$126]($122, $127, $128, $129, $131);
      label = 26; break;
    case 26: 
      label = 27; break;
    case 27: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function __ZNK10__cxxabiv117__class_type_info16search_below_dstEPNS_19__dynamic_cast_infoEPKvib($this, $info, $current_ptr, $path_below, $use_strcmp) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $5;
      $1=$this;
      $2=$info;
      $3=$current_ptr;
      $4=$path_below;
      var $6=(($use_strcmp)&(1));
      $5=$6;
      var $7=$1;
      var $8=$7;
      var $9=$2;
      var $10=(($9+8)|0);
      var $11=HEAP32[(($10)>>2)];
      var $12=$11;
      var $13=$5;
      var $14=(($13) & 1);
      var $15=__ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($8, $12, $14);
      if ($15) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $17=$2;
      var $18=$3;
      var $19=$4;
      __ZNK10__cxxabiv117__class_type_info29process_static_type_below_dstEPNS_19__dynamic_cast_infoEPKvi($7, $17, $18, $19);
      label = 15; break;
    case 3: 
      var $21=$7;
      var $22=$2;
      var $23=(($22)|0);
      var $24=HEAP32[(($23)>>2)];
      var $25=$24;
      var $26=$5;
      var $27=(($26) & 1);
      var $28=__ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($21, $25, $27);
      if ($28) { label = 4; break; } else { label = 14; break; }
    case 4: 
      var $30=$3;
      var $31=$2;
      var $32=(($31+16)|0);
      var $33=HEAP32[(($32)>>2)];
      var $34=(($30)|(0))==(($33)|(0));
      if ($34) { label = 6; break; } else { label = 5; break; }
    case 5: 
      var $36=$3;
      var $37=$2;
      var $38=(($37+20)|0);
      var $39=HEAP32[(($38)>>2)];
      var $40=(($36)|(0))==(($39)|(0));
      if ($40) { label = 6; break; } else { label = 9; break; }
    case 6: 
      var $42=$4;
      var $43=(($42)|(0))==1;
      if ($43) { label = 7; break; } else { label = 8; break; }
    case 7: 
      var $45=$2;
      var $46=(($45+32)|0);
      HEAP32[(($46)>>2)]=1;
      label = 8; break;
    case 8: 
      label = 13; break;
    case 9: 
      var $49=$4;
      var $50=$2;
      var $51=(($50+32)|0);
      HEAP32[(($51)>>2)]=$49;
      var $52=$3;
      var $53=$2;
      var $54=(($53+20)|0);
      HEAP32[(($54)>>2)]=$52;
      var $55=$2;
      var $56=(($55+40)|0);
      var $57=HEAP32[(($56)>>2)];
      var $58=((($57)+(1))|0);
      HEAP32[(($56)>>2)]=$58;
      var $59=$2;
      var $60=(($59+36)|0);
      var $61=HEAP32[(($60)>>2)];
      var $62=(($61)|(0))==1;
      if ($62) { label = 10; break; } else { label = 12; break; }
    case 10: 
      var $64=$2;
      var $65=(($64+24)|0);
      var $66=HEAP32[(($65)>>2)];
      var $67=(($66)|(0))==2;
      if ($67) { label = 11; break; } else { label = 12; break; }
    case 11: 
      var $69=$2;
      var $70=(($69+54)|0);
      HEAP8[($70)]=1;
      label = 12; break;
    case 12: 
      var $72=$2;
      var $73=(($72+44)|0);
      HEAP32[(($73)>>2)]=4;
      label = 13; break;
    case 13: 
      label = 14; break;
    case 14: 
      label = 15; break;
    case 15: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function __ZNK10__cxxabiv120__si_class_type_info16search_above_dstEPNS_19__dynamic_cast_infoEPKvS4_ib($this, $info, $dst_ptr, $current_ptr, $path_below, $use_strcmp) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $5;
      var $6;
      $1=$this;
      $2=$info;
      $3=$dst_ptr;
      $4=$current_ptr;
      $5=$path_below;
      var $7=(($use_strcmp)&(1));
      $6=$7;
      var $8=$1;
      var $9=$8;
      var $10=$2;
      var $11=(($10+8)|0);
      var $12=HEAP32[(($11)>>2)];
      var $13=$12;
      var $14=$6;
      var $15=(($14) & 1);
      var $16=__ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($9, $13, $15);
      if ($16) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $18=$8;
      var $19=$2;
      var $20=$3;
      var $21=$4;
      var $22=$5;
      __ZNK10__cxxabiv117__class_type_info29process_static_type_above_dstEPNS_19__dynamic_cast_infoEPKvS4_i($18, $19, $20, $21, $22);
      label = 4; break;
    case 3: 
      var $24=(($8+8)|0);
      var $25=HEAP32[(($24)>>2)];
      var $26=$25;
      var $27=HEAP32[(($26)>>2)];
      var $28=(($27+20)|0);
      var $29=HEAP32[(($28)>>2)];
      var $30=$2;
      var $31=$3;
      var $32=$4;
      var $33=$5;
      var $34=$6;
      var $35=(($34) & 1);
      FUNCTION_TABLE[$29]($25, $30, $31, $32, $33, $35);
      label = 4; break;
    case 4: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function __ZNK10__cxxabiv117__class_type_info16search_above_dstEPNS_19__dynamic_cast_infoEPKvS4_ib($this, $info, $dst_ptr, $current_ptr, $path_below, $use_strcmp) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $5;
      var $6;
      $1=$this;
      $2=$info;
      $3=$dst_ptr;
      $4=$current_ptr;
      $5=$path_below;
      var $7=(($use_strcmp)&(1));
      $6=$7;
      var $8=$1;
      var $9=$8;
      var $10=$2;
      var $11=(($10+8)|0);
      var $12=HEAP32[(($11)>>2)];
      var $13=$12;
      var $14=$6;
      var $15=(($14) & 1);
      var $16=__ZN10__cxxabiv18is_equalEPKSt9type_infoS2_b($9, $13, $15);
      if ($16) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $18=$2;
      var $19=$3;
      var $20=$4;
      var $21=$5;
      __ZNK10__cxxabiv117__class_type_info29process_static_type_above_dstEPNS_19__dynamic_cast_infoEPKvS4_i($8, $18, $19, $20, $21);
      label = 3; break;
    case 3: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function ___cxx_global_var_init47() {
  var label = 0;
  __ZN5ForceC1Ev(1696);
  return;
}
function __ZN5ForceC1Ev($this) {
  var label = 0;
  var $1;
  $1=$this;
  var $2=$1;
  __ZN5ForceC2Ev($2);
  return;
}
function __ZN5ForceC2Ev($this) {
  var label = 0;
  var $1;
  var $x;
  $1=$this;
  var $2=$1;
  var $3=_malloc(10);
  $x=$3;
  var $4=$x;
  _free($4);
  return;
}
function __GLOBAL__I_a48() {
  var label = 0;
  ___cxx_global_var_init47();
  return;
}
function _malloc($bytes) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $mem;
      var $nb;
      var $idx;
      var $smallbits;
      var $b;
      var $p;
      var $F;
      var $b1;
      var $p2;
      var $r;
      var $rsize;
      var $i;
      var $leftbits;
      var $leastbit;
      var $Y;
      var $K;
      var $N;
      var $F3;
      var $DVS;
      var $DV;
      var $I;
      var $B;
      var $F4;
      var $rsize5;
      var $p6;
      var $r7;
      var $dvs;
      var $rsize8;
      var $p9;
      var $r10;
      $1=$bytes;
      var $2=$1;
      var $3=(($2)>>>(0)) <= 244;
      if ($3) { label = 2; break; } else { label = 41; break; }
    case 2: 
      var $5=$1;
      var $6=(($5)>>>(0)) < 11;
      if ($6) { label = 3; break; } else { label = 4; break; }
    case 3: 
      var $14 = 16;label = 5; break;
    case 4: 
      var $9=$1;
      var $10=((($9)+(4))|0);
      var $11=((($10)+(7))|0);
      var $12=$11 & -8;
      var $14 = $12;label = 5; break;
    case 5: 
      var $14;
      $nb=$14;
      var $15=$nb;
      var $16=$15 >>> 3;
      $idx=$16;
      var $17=HEAP32[((((408)|0))>>2)];
      var $18=$idx;
      var $19=$17 >>> (($18)>>>(0));
      $smallbits=$19;
      var $20=$smallbits;
      var $21=$20 & 3;
      var $22=(($21)|(0))!=0;
      if ($22) { label = 6; break; } else { label = 15; break; }
    case 6: 
      var $24=$smallbits;
      var $25=$24 ^ -1;
      var $26=$25 & 1;
      var $27=$idx;
      var $28=((($27)+($26))|0);
      $idx=$28;
      var $29=$idx;
      var $30=$29 << 1;
      var $31=((((448)|0)+($30<<2))|0);
      var $32=$31;
      var $33=$32;
      $b=$33;
      var $34=$b;
      var $35=(($34+8)|0);
      var $36=HEAP32[(($35)>>2)];
      $p=$36;
      var $37=$p;
      var $38=(($37+8)|0);
      var $39=HEAP32[(($38)>>2)];
      $F=$39;
      var $40=$b;
      var $41=$F;
      var $42=(($40)|(0))==(($41)|(0));
      if ($42) { label = 7; break; } else { label = 8; break; }
    case 7: 
      var $44=$idx;
      var $45=1 << $44;
      var $46=$45 ^ -1;
      var $47=HEAP32[((((408)|0))>>2)];
      var $48=$47 & $46;
      HEAP32[((((408)|0))>>2)]=$48;
      label = 14; break;
    case 8: 
      var $50=$F;
      var $51=$50;
      var $52=HEAP32[((((424)|0))>>2)];
      var $53=(($51)>>>(0)) >= (($52)>>>(0));
      if ($53) { label = 9; break; } else { var $61 = 0;label = 10; break; }
    case 9: 
      var $55=$F;
      var $56=(($55+12)|0);
      var $57=HEAP32[(($56)>>2)];
      var $58=$p;
      var $59=(($57)|(0))==(($58)|(0));
      var $61 = $59;label = 10; break;
    case 10: 
      var $61;
      var $62=(($61)&(1));
      var $63=($62);
      var $64=(($63)|(0))!=0;
      if ($64) { label = 11; break; } else { label = 12; break; }
    case 11: 
      var $66=$b;
      var $67=$F;
      var $68=(($67+12)|0);
      HEAP32[(($68)>>2)]=$66;
      var $69=$F;
      var $70=$b;
      var $71=(($70+8)|0);
      HEAP32[(($71)>>2)]=$69;
      label = 13; break;
    case 12: 
      _abort();
      throw "Reached an unreachable!";
    case 13: 
      label = 14; break;
    case 14: 
      var $75=$idx;
      var $76=$75 << 3;
      var $77=$76 | 1;
      var $78=$77 | 2;
      var $79=$p;
      var $80=(($79+4)|0);
      HEAP32[(($80)>>2)]=$78;
      var $81=$p;
      var $82=$81;
      var $83=$idx;
      var $84=$83 << 3;
      var $85=(($82+$84)|0);
      var $86=$85;
      var $87=(($86+4)|0);
      var $88=HEAP32[(($87)>>2)];
      var $89=$88 | 1;
      HEAP32[(($87)>>2)]=$89;
      var $90=$p;
      var $91=$90;
      var $92=(($91+8)|0);
      $mem=$92;
      label = 57; break;
    case 15: 
      var $94=$nb;
      var $95=HEAP32[((((416)|0))>>2)];
      var $96=(($94)>>>(0)) > (($95)>>>(0));
      if ($96) { label = 16; break; } else { label = 39; break; }
    case 16: 
      var $98=$smallbits;
      var $99=(($98)|(0))!=0;
      if ($99) { label = 17; break; } else { label = 34; break; }
    case 17: 
      var $101=$smallbits;
      var $102=$idx;
      var $103=$101 << $102;
      var $104=$idx;
      var $105=1 << $104;
      var $106=$105 << 1;
      var $107=$idx;
      var $108=1 << $107;
      var $109=$108 << 1;
      var $110=(((-$109))|0);
      var $111=$106 | $110;
      var $112=$103 & $111;
      $leftbits=$112;
      var $113=$leftbits;
      var $114=$leftbits;
      var $115=(((-$114))|0);
      var $116=$113 & $115;
      $leastbit=$116;
      var $117=$leastbit;
      var $118=((($117)-(1))|0);
      $Y=$118;
      var $119=$Y;
      var $120=$119 >>> 12;
      var $121=$120 & 16;
      $K=$121;
      var $122=$K;
      $N=$122;
      var $123=$K;
      var $124=$Y;
      var $125=$124 >>> (($123)>>>(0));
      $Y=$125;
      var $126=$Y;
      var $127=$126 >>> 5;
      var $128=$127 & 8;
      $K=$128;
      var $129=$N;
      var $130=((($129)+($128))|0);
      $N=$130;
      var $131=$K;
      var $132=$Y;
      var $133=$132 >>> (($131)>>>(0));
      $Y=$133;
      var $134=$Y;
      var $135=$134 >>> 2;
      var $136=$135 & 4;
      $K=$136;
      var $137=$N;
      var $138=((($137)+($136))|0);
      $N=$138;
      var $139=$K;
      var $140=$Y;
      var $141=$140 >>> (($139)>>>(0));
      $Y=$141;
      var $142=$Y;
      var $143=$142 >>> 1;
      var $144=$143 & 2;
      $K=$144;
      var $145=$N;
      var $146=((($145)+($144))|0);
      $N=$146;
      var $147=$K;
      var $148=$Y;
      var $149=$148 >>> (($147)>>>(0));
      $Y=$149;
      var $150=$Y;
      var $151=$150 >>> 1;
      var $152=$151 & 1;
      $K=$152;
      var $153=$N;
      var $154=((($153)+($152))|0);
      $N=$154;
      var $155=$K;
      var $156=$Y;
      var $157=$156 >>> (($155)>>>(0));
      $Y=$157;
      var $158=$N;
      var $159=$Y;
      var $160=((($158)+($159))|0);
      $i=$160;
      var $161=$i;
      var $162=$161 << 1;
      var $163=((((448)|0)+($162<<2))|0);
      var $164=$163;
      var $165=$164;
      $b1=$165;
      var $166=$b1;
      var $167=(($166+8)|0);
      var $168=HEAP32[(($167)>>2)];
      $p2=$168;
      var $169=$p2;
      var $170=(($169+8)|0);
      var $171=HEAP32[(($170)>>2)];
      $F3=$171;
      var $172=$b1;
      var $173=$F3;
      var $174=(($172)|(0))==(($173)|(0));
      if ($174) { label = 18; break; } else { label = 19; break; }
    case 18: 
      var $176=$i;
      var $177=1 << $176;
      var $178=$177 ^ -1;
      var $179=HEAP32[((((408)|0))>>2)];
      var $180=$179 & $178;
      HEAP32[((((408)|0))>>2)]=$180;
      label = 25; break;
    case 19: 
      var $182=$F3;
      var $183=$182;
      var $184=HEAP32[((((424)|0))>>2)];
      var $185=(($183)>>>(0)) >= (($184)>>>(0));
      if ($185) { label = 20; break; } else { var $193 = 0;label = 21; break; }
    case 20: 
      var $187=$F3;
      var $188=(($187+12)|0);
      var $189=HEAP32[(($188)>>2)];
      var $190=$p2;
      var $191=(($189)|(0))==(($190)|(0));
      var $193 = $191;label = 21; break;
    case 21: 
      var $193;
      var $194=(($193)&(1));
      var $195=($194);
      var $196=(($195)|(0))!=0;
      if ($196) { label = 22; break; } else { label = 23; break; }
    case 22: 
      var $198=$b1;
      var $199=$F3;
      var $200=(($199+12)|0);
      HEAP32[(($200)>>2)]=$198;
      var $201=$F3;
      var $202=$b1;
      var $203=(($202+8)|0);
      HEAP32[(($203)>>2)]=$201;
      label = 24; break;
    case 23: 
      _abort();
      throw "Reached an unreachable!";
    case 24: 
      label = 25; break;
    case 25: 
      var $207=$i;
      var $208=$207 << 3;
      var $209=$nb;
      var $210=((($208)-($209))|0);
      $rsize=$210;
      var $211=$nb;
      var $212=$211 | 1;
      var $213=$212 | 2;
      var $214=$p2;
      var $215=(($214+4)|0);
      HEAP32[(($215)>>2)]=$213;
      var $216=$p2;
      var $217=$216;
      var $218=$nb;
      var $219=(($217+$218)|0);
      var $220=$219;
      $r=$220;
      var $221=$rsize;
      var $222=$221 | 1;
      var $223=$r;
      var $224=(($223+4)|0);
      HEAP32[(($224)>>2)]=$222;
      var $225=$rsize;
      var $226=$r;
      var $227=$226;
      var $228=$rsize;
      var $229=(($227+$228)|0);
      var $230=$229;
      var $231=(($230)|0);
      HEAP32[(($231)>>2)]=$225;
      var $232=HEAP32[((((416)|0))>>2)];
      $DVS=$232;
      var $233=$DVS;
      var $234=(($233)|(0))!=0;
      if ($234) { label = 26; break; } else { label = 33; break; }
    case 26: 
      var $236=HEAP32[((((428)|0))>>2)];
      $DV=$236;
      var $237=$DVS;
      var $238=$237 >>> 3;
      $I=$238;
      var $239=$I;
      var $240=$239 << 1;
      var $241=((((448)|0)+($240<<2))|0);
      var $242=$241;
      var $243=$242;
      $B=$243;
      var $244=$B;
      $F4=$244;
      var $245=HEAP32[((((408)|0))>>2)];
      var $246=$I;
      var $247=1 << $246;
      var $248=$245 & $247;
      var $249=(($248)|(0))!=0;
      if ($249) { label = 28; break; } else { label = 27; break; }
    case 27: 
      var $251=$I;
      var $252=1 << $251;
      var $253=HEAP32[((((408)|0))>>2)];
      var $254=$253 | $252;
      HEAP32[((((408)|0))>>2)]=$254;
      label = 32; break;
    case 28: 
      var $256=$B;
      var $257=(($256+8)|0);
      var $258=HEAP32[(($257)>>2)];
      var $259=$258;
      var $260=HEAP32[((((424)|0))>>2)];
      var $261=(($259)>>>(0)) >= (($260)>>>(0));
      var $262=(($261)&(1));
      var $263=($262);
      var $264=(($263)|(0))!=0;
      if ($264) { label = 29; break; } else { label = 30; break; }
    case 29: 
      var $266=$B;
      var $267=(($266+8)|0);
      var $268=HEAP32[(($267)>>2)];
      $F4=$268;
      label = 31; break;
    case 30: 
      _abort();
      throw "Reached an unreachable!";
    case 31: 
      label = 32; break;
    case 32: 
      var $272=$DV;
      var $273=$B;
      var $274=(($273+8)|0);
      HEAP32[(($274)>>2)]=$272;
      var $275=$DV;
      var $276=$F4;
      var $277=(($276+12)|0);
      HEAP32[(($277)>>2)]=$275;
      var $278=$F4;
      var $279=$DV;
      var $280=(($279+8)|0);
      HEAP32[(($280)>>2)]=$278;
      var $281=$B;
      var $282=$DV;
      var $283=(($282+12)|0);
      HEAP32[(($283)>>2)]=$281;
      label = 33; break;
    case 33: 
      var $285=$rsize;
      HEAP32[((((416)|0))>>2)]=$285;
      var $286=$r;
      HEAP32[((((428)|0))>>2)]=$286;
      var $287=$p2;
      var $288=$287;
      var $289=(($288+8)|0);
      $mem=$289;
      label = 57; break;
    case 34: 
      var $291=HEAP32[((((412)|0))>>2)];
      var $292=(($291)|(0))!=0;
      if ($292) { label = 35; break; } else { label = 37; break; }
    case 35: 
      var $294=$nb;
      var $295=_tmalloc_small(408, $294);
      $mem=$295;
      var $296=(($295)|(0))!=0;
      if ($296) { label = 36; break; } else { label = 37; break; }
    case 36: 
      label = 57; break;
    case 37: 
      label = 38; break;
    case 38: 
      label = 39; break;
    case 39: 
      label = 40; break;
    case 40: 
      label = 48; break;
    case 41: 
      var $303=$1;
      var $304=(($303)>>>(0)) >= 4294967232;
      if ($304) { label = 42; break; } else { label = 43; break; }
    case 42: 
      $nb=-1;
      label = 47; break;
    case 43: 
      var $307=$1;
      var $308=((($307)+(4))|0);
      var $309=((($308)+(7))|0);
      var $310=$309 & -8;
      $nb=$310;
      var $311=HEAP32[((((412)|0))>>2)];
      var $312=(($311)|(0))!=0;
      if ($312) { label = 44; break; } else { label = 46; break; }
    case 44: 
      var $314=$nb;
      var $315=_tmalloc_large(408, $314);
      $mem=$315;
      var $316=(($315)|(0))!=0;
      if ($316) { label = 45; break; } else { label = 46; break; }
    case 45: 
      label = 57; break;
    case 46: 
      label = 47; break;
    case 47: 
      label = 48; break;
    case 48: 
      var $321=$nb;
      var $322=HEAP32[((((416)|0))>>2)];
      var $323=(($321)>>>(0)) <= (($322)>>>(0));
      if ($323) { label = 49; break; } else { label = 53; break; }
    case 49: 
      var $325=HEAP32[((((416)|0))>>2)];
      var $326=$nb;
      var $327=((($325)-($326))|0);
      $rsize5=$327;
      var $328=HEAP32[((((428)|0))>>2)];
      $p6=$328;
      var $329=$rsize5;
      var $330=(($329)>>>(0)) >= 16;
      if ($330) { label = 50; break; } else { label = 51; break; }
    case 50: 
      var $332=$p6;
      var $333=$332;
      var $334=$nb;
      var $335=(($333+$334)|0);
      var $336=$335;
      HEAP32[((((428)|0))>>2)]=$336;
      $r7=$336;
      var $337=$rsize5;
      HEAP32[((((416)|0))>>2)]=$337;
      var $338=$rsize5;
      var $339=$338 | 1;
      var $340=$r7;
      var $341=(($340+4)|0);
      HEAP32[(($341)>>2)]=$339;
      var $342=$rsize5;
      var $343=$r7;
      var $344=$343;
      var $345=$rsize5;
      var $346=(($344+$345)|0);
      var $347=$346;
      var $348=(($347)|0);
      HEAP32[(($348)>>2)]=$342;
      var $349=$nb;
      var $350=$349 | 1;
      var $351=$350 | 2;
      var $352=$p6;
      var $353=(($352+4)|0);
      HEAP32[(($353)>>2)]=$351;
      label = 52; break;
    case 51: 
      var $355=HEAP32[((((416)|0))>>2)];
      $dvs=$355;
      HEAP32[((((416)|0))>>2)]=0;
      HEAP32[((((428)|0))>>2)]=0;
      var $356=$dvs;
      var $357=$356 | 1;
      var $358=$357 | 2;
      var $359=$p6;
      var $360=(($359+4)|0);
      HEAP32[(($360)>>2)]=$358;
      var $361=$p6;
      var $362=$361;
      var $363=$dvs;
      var $364=(($362+$363)|0);
      var $365=$364;
      var $366=(($365+4)|0);
      var $367=HEAP32[(($366)>>2)];
      var $368=$367 | 1;
      HEAP32[(($366)>>2)]=$368;
      label = 52; break;
    case 52: 
      var $370=$p6;
      var $371=$370;
      var $372=(($371+8)|0);
      $mem=$372;
      label = 57; break;
    case 53: 
      var $374=$nb;
      var $375=HEAP32[((((420)|0))>>2)];
      var $376=(($374)>>>(0)) < (($375)>>>(0));
      if ($376) { label = 54; break; } else { label = 55; break; }
    case 54: 
      var $378=$nb;
      var $379=HEAP32[((((420)|0))>>2)];
      var $380=((($379)-($378))|0);
      HEAP32[((((420)|0))>>2)]=$380;
      $rsize8=$380;
      var $381=HEAP32[((((432)|0))>>2)];
      $p9=$381;
      var $382=$p9;
      var $383=$382;
      var $384=$nb;
      var $385=(($383+$384)|0);
      var $386=$385;
      HEAP32[((((432)|0))>>2)]=$386;
      $r10=$386;
      var $387=$rsize8;
      var $388=$387 | 1;
      var $389=$r10;
      var $390=(($389+4)|0);
      HEAP32[(($390)>>2)]=$388;
      var $391=$nb;
      var $392=$391 | 1;
      var $393=$392 | 2;
      var $394=$p9;
      var $395=(($394+4)|0);
      HEAP32[(($395)>>2)]=$393;
      var $396=$p9;
      var $397=$396;
      var $398=(($397+8)|0);
      $mem=$398;
      label = 57; break;
    case 55: 
      label = 56; break;
    case 56: 
      var $401=$nb;
      var $402=_sys_alloc(408, $401);
      $mem=$402;
      label = 57; break;
    case 57: 
      var $404=$mem;
      return $404;
    default: assert(0, "bad label: " + label);
  }
}
Module["_malloc"] = _malloc;
function _tmalloc_small($m, $nb) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $t;
      var $v;
      var $rsize;
      var $i;
      var $leastbit;
      var $Y;
      var $K;
      var $N;
      var $trem;
      var $r;
      var $XP;
      var $R;
      var $F;
      var $RP;
      var $CP;
      var $H;
      var $C0;
      var $C1;
      var $DVS;
      var $DV;
      var $I;
      var $B;
      var $F1;
      $1=$m;
      $2=$nb;
      var $3=$1;
      var $4=(($3+4)|0);
      var $5=HEAP32[(($4)>>2)];
      var $6=$1;
      var $7=(($6+4)|0);
      var $8=HEAP32[(($7)>>2)];
      var $9=(((-$8))|0);
      var $10=$5 & $9;
      $leastbit=$10;
      var $11=$leastbit;
      var $12=((($11)-(1))|0);
      $Y=$12;
      var $13=$Y;
      var $14=$13 >>> 12;
      var $15=$14 & 16;
      $K=$15;
      var $16=$K;
      $N=$16;
      var $17=$K;
      var $18=$Y;
      var $19=$18 >>> (($17)>>>(0));
      $Y=$19;
      var $20=$Y;
      var $21=$20 >>> 5;
      var $22=$21 & 8;
      $K=$22;
      var $23=$N;
      var $24=((($23)+($22))|0);
      $N=$24;
      var $25=$K;
      var $26=$Y;
      var $27=$26 >>> (($25)>>>(0));
      $Y=$27;
      var $28=$Y;
      var $29=$28 >>> 2;
      var $30=$29 & 4;
      $K=$30;
      var $31=$N;
      var $32=((($31)+($30))|0);
      $N=$32;
      var $33=$K;
      var $34=$Y;
      var $35=$34 >>> (($33)>>>(0));
      $Y=$35;
      var $36=$Y;
      var $37=$36 >>> 1;
      var $38=$37 & 2;
      $K=$38;
      var $39=$N;
      var $40=((($39)+($38))|0);
      $N=$40;
      var $41=$K;
      var $42=$Y;
      var $43=$42 >>> (($41)>>>(0));
      $Y=$43;
      var $44=$Y;
      var $45=$44 >>> 1;
      var $46=$45 & 1;
      $K=$46;
      var $47=$N;
      var $48=((($47)+($46))|0);
      $N=$48;
      var $49=$K;
      var $50=$Y;
      var $51=$50 >>> (($49)>>>(0));
      $Y=$51;
      var $52=$N;
      var $53=$Y;
      var $54=((($52)+($53))|0);
      $i=$54;
      var $55=$i;
      var $56=$1;
      var $57=(($56+304)|0);
      var $58=(($57+($55<<2))|0);
      var $59=HEAP32[(($58)>>2)];
      $t=$59;
      $v=$59;
      var $60=$t;
      var $61=(($60+4)|0);
      var $62=HEAP32[(($61)>>2)];
      var $63=$62 & -8;
      var $64=$2;
      var $65=((($63)-($64))|0);
      $rsize=$65;
      label = 2; break;
    case 2: 
      var $67=$t;
      var $68=(($67+16)|0);
      var $69=(($68)|0);
      var $70=HEAP32[(($69)>>2)];
      var $71=(($70)|(0))!=0;
      if ($71) { label = 3; break; } else { label = 4; break; }
    case 3: 
      var $73=$t;
      var $74=(($73+16)|0);
      var $75=(($74)|0);
      var $76=HEAP32[(($75)>>2)];
      var $83 = $76;label = 5; break;
    case 4: 
      var $78=$t;
      var $79=(($78+16)|0);
      var $80=(($79+4)|0);
      var $81=HEAP32[(($80)>>2)];
      var $83 = $81;label = 5; break;
    case 5: 
      var $83;
      $t=$83;
      var $84=(($83)|(0))!=0;
      if ($84) { label = 6; break; } else { label = 9; break; }
    case 6: 
      var $86=$t;
      var $87=(($86+4)|0);
      var $88=HEAP32[(($87)>>2)];
      var $89=$88 & -8;
      var $90=$2;
      var $91=((($89)-($90))|0);
      $trem=$91;
      var $92=$trem;
      var $93=$rsize;
      var $94=(($92)>>>(0)) < (($93)>>>(0));
      if ($94) { label = 7; break; } else { label = 8; break; }
    case 7: 
      var $96=$trem;
      $rsize=$96;
      var $97=$t;
      $v=$97;
      label = 8; break;
    case 8: 
      label = 2; break;
    case 9: 
      var $100=$v;
      var $101=$100;
      var $102=$1;
      var $103=(($102+16)|0);
      var $104=HEAP32[(($103)>>2)];
      var $105=(($101)>>>(0)) >= (($104)>>>(0));
      var $106=(($105)&(1));
      var $107=($106);
      var $108=(($107)|(0))!=0;
      if ($108) { label = 10; break; } else { label = 72; break; }
    case 10: 
      var $110=$v;
      var $111=$110;
      var $112=$2;
      var $113=(($111+$112)|0);
      var $114=$113;
      $r=$114;
      var $115=$v;
      var $116=$115;
      var $117=$r;
      var $118=$117;
      var $119=(($116)>>>(0)) < (($118)>>>(0));
      var $120=(($119)&(1));
      var $121=($120);
      var $122=(($121)|(0))!=0;
      if ($122) { label = 11; break; } else { label = 71; break; }
    case 11: 
      var $124=$v;
      var $125=(($124+24)|0);
      var $126=HEAP32[(($125)>>2)];
      $XP=$126;
      var $127=$v;
      var $128=(($127+12)|0);
      var $129=HEAP32[(($128)>>2)];
      var $130=$v;
      var $131=(($129)|(0))!=(($130)|(0));
      if ($131) { label = 12; break; } else { label = 19; break; }
    case 12: 
      var $133=$v;
      var $134=(($133+8)|0);
      var $135=HEAP32[(($134)>>2)];
      $F=$135;
      var $136=$v;
      var $137=(($136+12)|0);
      var $138=HEAP32[(($137)>>2)];
      $R=$138;
      var $139=$F;
      var $140=$139;
      var $141=$1;
      var $142=(($141+16)|0);
      var $143=HEAP32[(($142)>>2)];
      var $144=(($140)>>>(0)) >= (($143)>>>(0));
      if ($144) { label = 13; break; } else { var $158 = 0;label = 15; break; }
    case 13: 
      var $146=$F;
      var $147=(($146+12)|0);
      var $148=HEAP32[(($147)>>2)];
      var $149=$v;
      var $150=(($148)|(0))==(($149)|(0));
      if ($150) { label = 14; break; } else { var $158 = 0;label = 15; break; }
    case 14: 
      var $152=$R;
      var $153=(($152+8)|0);
      var $154=HEAP32[(($153)>>2)];
      var $155=$v;
      var $156=(($154)|(0))==(($155)|(0));
      var $158 = $156;label = 15; break;
    case 15: 
      var $158;
      var $159=(($158)&(1));
      var $160=($159);
      var $161=(($160)|(0))!=0;
      if ($161) { label = 16; break; } else { label = 17; break; }
    case 16: 
      var $163=$R;
      var $164=$F;
      var $165=(($164+12)|0);
      HEAP32[(($165)>>2)]=$163;
      var $166=$F;
      var $167=$R;
      var $168=(($167+8)|0);
      HEAP32[(($168)>>2)]=$166;
      label = 18; break;
    case 17: 
      _abort();
      throw "Reached an unreachable!";
    case 18: 
      label = 31; break;
    case 19: 
      var $172=$v;
      var $173=(($172+16)|0);
      var $174=(($173+4)|0);
      $RP=$174;
      var $175=HEAP32[(($174)>>2)];
      $R=$175;
      var $176=(($175)|(0))!=0;
      if ($176) { label = 21; break; } else { label = 20; break; }
    case 20: 
      var $178=$v;
      var $179=(($178+16)|0);
      var $180=(($179)|0);
      $RP=$180;
      var $181=HEAP32[(($180)>>2)];
      $R=$181;
      var $182=(($181)|(0))!=0;
      if ($182) { label = 21; break; } else { label = 30; break; }
    case 21: 
      label = 22; break;
    case 22: 
      var $185=$R;
      var $186=(($185+16)|0);
      var $187=(($186+4)|0);
      $CP=$187;
      var $188=HEAP32[(($187)>>2)];
      var $189=(($188)|(0))!=0;
      if ($189) { var $197 = 1;label = 24; break; } else { label = 23; break; }
    case 23: 
      var $191=$R;
      var $192=(($191+16)|0);
      var $193=(($192)|0);
      $CP=$193;
      var $194=HEAP32[(($193)>>2)];
      var $195=(($194)|(0))!=0;
      var $197 = $195;label = 24; break;
    case 24: 
      var $197;
      if ($197) { label = 25; break; } else { label = 26; break; }
    case 25: 
      var $199=$CP;
      $RP=$199;
      var $200=HEAP32[(($199)>>2)];
      $R=$200;
      label = 22; break;
    case 26: 
      var $202=$RP;
      var $203=$202;
      var $204=$1;
      var $205=(($204+16)|0);
      var $206=HEAP32[(($205)>>2)];
      var $207=(($203)>>>(0)) >= (($206)>>>(0));
      var $208=(($207)&(1));
      var $209=($208);
      var $210=(($209)|(0))!=0;
      if ($210) { label = 27; break; } else { label = 28; break; }
    case 27: 
      var $212=$RP;
      HEAP32[(($212)>>2)]=0;
      label = 29; break;
    case 28: 
      _abort();
      throw "Reached an unreachable!";
    case 29: 
      label = 30; break;
    case 30: 
      label = 31; break;
    case 31: 
      var $217=$XP;
      var $218=(($217)|(0))!=0;
      if ($218) { label = 32; break; } else { label = 59; break; }
    case 32: 
      var $220=$v;
      var $221=(($220+28)|0);
      var $222=HEAP32[(($221)>>2)];
      var $223=$1;
      var $224=(($223+304)|0);
      var $225=(($224+($222<<2))|0);
      $H=$225;
      var $226=$v;
      var $227=$H;
      var $228=HEAP32[(($227)>>2)];
      var $229=(($226)|(0))==(($228)|(0));
      if ($229) { label = 33; break; } else { label = 36; break; }
    case 33: 
      var $231=$R;
      var $232=$H;
      HEAP32[(($232)>>2)]=$231;
      var $233=(($231)|(0))==0;
      if ($233) { label = 34; break; } else { label = 35; break; }
    case 34: 
      var $235=$v;
      var $236=(($235+28)|0);
      var $237=HEAP32[(($236)>>2)];
      var $238=1 << $237;
      var $239=$238 ^ -1;
      var $240=$1;
      var $241=(($240+4)|0);
      var $242=HEAP32[(($241)>>2)];
      var $243=$242 & $239;
      HEAP32[(($241)>>2)]=$243;
      label = 35; break;
    case 35: 
      label = 43; break;
    case 36: 
      var $246=$XP;
      var $247=$246;
      var $248=$1;
      var $249=(($248+16)|0);
      var $250=HEAP32[(($249)>>2)];
      var $251=(($247)>>>(0)) >= (($250)>>>(0));
      var $252=(($251)&(1));
      var $253=($252);
      var $254=(($253)|(0))!=0;
      if ($254) { label = 37; break; } else { label = 41; break; }
    case 37: 
      var $256=$XP;
      var $257=(($256+16)|0);
      var $258=(($257)|0);
      var $259=HEAP32[(($258)>>2)];
      var $260=$v;
      var $261=(($259)|(0))==(($260)|(0));
      if ($261) { label = 38; break; } else { label = 39; break; }
    case 38: 
      var $263=$R;
      var $264=$XP;
      var $265=(($264+16)|0);
      var $266=(($265)|0);
      HEAP32[(($266)>>2)]=$263;
      label = 40; break;
    case 39: 
      var $268=$R;
      var $269=$XP;
      var $270=(($269+16)|0);
      var $271=(($270+4)|0);
      HEAP32[(($271)>>2)]=$268;
      label = 40; break;
    case 40: 
      label = 42; break;
    case 41: 
      _abort();
      throw "Reached an unreachable!";
    case 42: 
      label = 43; break;
    case 43: 
      var $276=$R;
      var $277=(($276)|(0))!=0;
      if ($277) { label = 44; break; } else { label = 58; break; }
    case 44: 
      var $279=$R;
      var $280=$279;
      var $281=$1;
      var $282=(($281+16)|0);
      var $283=HEAP32[(($282)>>2)];
      var $284=(($280)>>>(0)) >= (($283)>>>(0));
      var $285=(($284)&(1));
      var $286=($285);
      var $287=(($286)|(0))!=0;
      if ($287) { label = 45; break; } else { label = 56; break; }
    case 45: 
      var $289=$XP;
      var $290=$R;
      var $291=(($290+24)|0);
      HEAP32[(($291)>>2)]=$289;
      var $292=$v;
      var $293=(($292+16)|0);
      var $294=(($293)|0);
      var $295=HEAP32[(($294)>>2)];
      $C0=$295;
      var $296=(($295)|(0))!=0;
      if ($296) { label = 46; break; } else { label = 50; break; }
    case 46: 
      var $298=$C0;
      var $299=$298;
      var $300=$1;
      var $301=(($300+16)|0);
      var $302=HEAP32[(($301)>>2)];
      var $303=(($299)>>>(0)) >= (($302)>>>(0));
      var $304=(($303)&(1));
      var $305=($304);
      var $306=(($305)|(0))!=0;
      if ($306) { label = 47; break; } else { label = 48; break; }
    case 47: 
      var $308=$C0;
      var $309=$R;
      var $310=(($309+16)|0);
      var $311=(($310)|0);
      HEAP32[(($311)>>2)]=$308;
      var $312=$R;
      var $313=$C0;
      var $314=(($313+24)|0);
      HEAP32[(($314)>>2)]=$312;
      label = 49; break;
    case 48: 
      _abort();
      throw "Reached an unreachable!";
    case 49: 
      label = 50; break;
    case 50: 
      var $318=$v;
      var $319=(($318+16)|0);
      var $320=(($319+4)|0);
      var $321=HEAP32[(($320)>>2)];
      $C1=$321;
      var $322=(($321)|(0))!=0;
      if ($322) { label = 51; break; } else { label = 55; break; }
    case 51: 
      var $324=$C1;
      var $325=$324;
      var $326=$1;
      var $327=(($326+16)|0);
      var $328=HEAP32[(($327)>>2)];
      var $329=(($325)>>>(0)) >= (($328)>>>(0));
      var $330=(($329)&(1));
      var $331=($330);
      var $332=(($331)|(0))!=0;
      if ($332) { label = 52; break; } else { label = 53; break; }
    case 52: 
      var $334=$C1;
      var $335=$R;
      var $336=(($335+16)|0);
      var $337=(($336+4)|0);
      HEAP32[(($337)>>2)]=$334;
      var $338=$R;
      var $339=$C1;
      var $340=(($339+24)|0);
      HEAP32[(($340)>>2)]=$338;
      label = 54; break;
    case 53: 
      _abort();
      throw "Reached an unreachable!";
    case 54: 
      label = 55; break;
    case 55: 
      label = 57; break;
    case 56: 
      _abort();
      throw "Reached an unreachable!";
    case 57: 
      label = 58; break;
    case 58: 
      label = 59; break;
    case 59: 
      var $348=$rsize;
      var $349=(($348)>>>(0)) < 16;
      if ($349) { label = 60; break; } else { label = 61; break; }
    case 60: 
      var $351=$rsize;
      var $352=$2;
      var $353=((($351)+($352))|0);
      var $354=$353 | 1;
      var $355=$354 | 2;
      var $356=$v;
      var $357=(($356+4)|0);
      HEAP32[(($357)>>2)]=$355;
      var $358=$v;
      var $359=$358;
      var $360=$rsize;
      var $361=$2;
      var $362=((($360)+($361))|0);
      var $363=(($359+$362)|0);
      var $364=$363;
      var $365=(($364+4)|0);
      var $366=HEAP32[(($365)>>2)];
      var $367=$366 | 1;
      HEAP32[(($365)>>2)]=$367;
      label = 70; break;
    case 61: 
      var $369=$2;
      var $370=$369 | 1;
      var $371=$370 | 2;
      var $372=$v;
      var $373=(($372+4)|0);
      HEAP32[(($373)>>2)]=$371;
      var $374=$rsize;
      var $375=$374 | 1;
      var $376=$r;
      var $377=(($376+4)|0);
      HEAP32[(($377)>>2)]=$375;
      var $378=$rsize;
      var $379=$r;
      var $380=$379;
      var $381=$rsize;
      var $382=(($380+$381)|0);
      var $383=$382;
      var $384=(($383)|0);
      HEAP32[(($384)>>2)]=$378;
      var $385=$1;
      var $386=(($385+8)|0);
      var $387=HEAP32[(($386)>>2)];
      $DVS=$387;
      var $388=$DVS;
      var $389=(($388)|(0))!=0;
      if ($389) { label = 62; break; } else { label = 69; break; }
    case 62: 
      var $391=$1;
      var $392=(($391+20)|0);
      var $393=HEAP32[(($392)>>2)];
      $DV=$393;
      var $394=$DVS;
      var $395=$394 >>> 3;
      $I=$395;
      var $396=$I;
      var $397=$396 << 1;
      var $398=$1;
      var $399=(($398+40)|0);
      var $400=(($399+($397<<2))|0);
      var $401=$400;
      var $402=$401;
      $B=$402;
      var $403=$B;
      $F1=$403;
      var $404=$1;
      var $405=(($404)|0);
      var $406=HEAP32[(($405)>>2)];
      var $407=$I;
      var $408=1 << $407;
      var $409=$406 & $408;
      var $410=(($409)|(0))!=0;
      if ($410) { label = 64; break; } else { label = 63; break; }
    case 63: 
      var $412=$I;
      var $413=1 << $412;
      var $414=$1;
      var $415=(($414)|0);
      var $416=HEAP32[(($415)>>2)];
      var $417=$416 | $413;
      HEAP32[(($415)>>2)]=$417;
      label = 68; break;
    case 64: 
      var $419=$B;
      var $420=(($419+8)|0);
      var $421=HEAP32[(($420)>>2)];
      var $422=$421;
      var $423=$1;
      var $424=(($423+16)|0);
      var $425=HEAP32[(($424)>>2)];
      var $426=(($422)>>>(0)) >= (($425)>>>(0));
      var $427=(($426)&(1));
      var $428=($427);
      var $429=(($428)|(0))!=0;
      if ($429) { label = 65; break; } else { label = 66; break; }
    case 65: 
      var $431=$B;
      var $432=(($431+8)|0);
      var $433=HEAP32[(($432)>>2)];
      $F1=$433;
      label = 67; break;
    case 66: 
      _abort();
      throw "Reached an unreachable!";
    case 67: 
      label = 68; break;
    case 68: 
      var $437=$DV;
      var $438=$B;
      var $439=(($438+8)|0);
      HEAP32[(($439)>>2)]=$437;
      var $440=$DV;
      var $441=$F1;
      var $442=(($441+12)|0);
      HEAP32[(($442)>>2)]=$440;
      var $443=$F1;
      var $444=$DV;
      var $445=(($444+8)|0);
      HEAP32[(($445)>>2)]=$443;
      var $446=$B;
      var $447=$DV;
      var $448=(($447+12)|0);
      HEAP32[(($448)>>2)]=$446;
      label = 69; break;
    case 69: 
      var $450=$rsize;
      var $451=$1;
      var $452=(($451+8)|0);
      HEAP32[(($452)>>2)]=$450;
      var $453=$r;
      var $454=$1;
      var $455=(($454+20)|0);
      HEAP32[(($455)>>2)]=$453;
      label = 70; break;
    case 70: 
      var $457=$v;
      var $458=$457;
      var $459=(($458+8)|0);
      return $459;
    case 71: 
      label = 72; break;
    case 72: 
      _abort();
      throw "Reached an unreachable!";
    default: assert(0, "bad label: " + label);
  }
}
function _tmalloc_large($m, $nb) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $v;
      var $rsize;
      var $t;
      var $idx;
      var $X;
      var $Y;
      var $N;
      var $K;
      var $sizebits;
      var $rst;
      var $rt;
      var $trem;
      var $leftbits;
      var $i;
      var $leastbit;
      var $Y1;
      var $K2;
      var $N3;
      var $trem4;
      var $r;
      var $XP;
      var $R;
      var $F;
      var $RP;
      var $CP;
      var $H;
      var $C0;
      var $C1;
      var $I;
      var $B;
      var $F5;
      var $TP;
      var $H6;
      var $I7;
      var $X8;
      var $Y9;
      var $N10;
      var $K11;
      var $T;
      var $K12;
      var $C;
      var $F13;
      $2=$m;
      $3=$nb;
      $v=0;
      var $4=$3;
      var $5=(((-$4))|0);
      $rsize=$5;
      var $6=$3;
      var $7=$6 >>> 8;
      $X=$7;
      var $8=$X;
      var $9=(($8)|(0))==0;
      if ($9) { label = 2; break; } else { label = 3; break; }
    case 2: 
      $idx=0;
      label = 7; break;
    case 3: 
      var $12=$X;
      var $13=(($12)>>>(0)) > 65535;
      if ($13) { label = 4; break; } else { label = 5; break; }
    case 4: 
      $idx=31;
      label = 6; break;
    case 5: 
      var $16=$X;
      $Y=$16;
      var $17=$Y;
      var $18=((($17)-(256))|0);
      var $19=$18 >>> 16;
      var $20=$19 & 8;
      $N=$20;
      var $21=$N;
      var $22=$Y;
      var $23=$22 << $21;
      $Y=$23;
      var $24=((($23)-(4096))|0);
      var $25=$24 >>> 16;
      var $26=$25 & 4;
      $K=$26;
      var $27=$K;
      var $28=$N;
      var $29=((($28)+($27))|0);
      $N=$29;
      var $30=$K;
      var $31=$Y;
      var $32=$31 << $30;
      $Y=$32;
      var $33=((($32)-(16384))|0);
      var $34=$33 >>> 16;
      var $35=$34 & 2;
      $K=$35;
      var $36=$N;
      var $37=((($36)+($35))|0);
      $N=$37;
      var $38=$N;
      var $39=(((14)-($38))|0);
      var $40=$K;
      var $41=$Y;
      var $42=$41 << $40;
      $Y=$42;
      var $43=$42 >>> 15;
      var $44=((($39)+($43))|0);
      $K=$44;
      var $45=$K;
      var $46=$45 << 1;
      var $47=$3;
      var $48=$K;
      var $49=((($48)+(7))|0);
      var $50=$47 >>> (($49)>>>(0));
      var $51=$50 & 1;
      var $52=((($46)+($51))|0);
      $idx=$52;
      label = 6; break;
    case 6: 
      label = 7; break;
    case 7: 
      var $55=$idx;
      var $56=$2;
      var $57=(($56+304)|0);
      var $58=(($57+($55<<2))|0);
      var $59=HEAP32[(($58)>>2)];
      $t=$59;
      var $60=(($59)|(0))!=0;
      if ($60) { label = 8; break; } else { label = 23; break; }
    case 8: 
      var $62=$3;
      var $63=$idx;
      var $64=(($63)|(0))==31;
      if ($64) { label = 9; break; } else { label = 10; break; }
    case 9: 
      var $73 = 0;label = 11; break;
    case 10: 
      var $67=$idx;
      var $68=$67 >>> 1;
      var $69=((($68)+(8))|0);
      var $70=((($69)-(2))|0);
      var $71=(((31)-($70))|0);
      var $73 = $71;label = 11; break;
    case 11: 
      var $73;
      var $74=$62 << $73;
      $sizebits=$74;
      $rst=0;
      label = 12; break;
    case 12: 
      var $76=$t;
      var $77=(($76+4)|0);
      var $78=HEAP32[(($77)>>2)];
      var $79=$78 & -8;
      var $80=$3;
      var $81=((($79)-($80))|0);
      $trem=$81;
      var $82=$trem;
      var $83=$rsize;
      var $84=(($82)>>>(0)) < (($83)>>>(0));
      if ($84) { label = 13; break; } else { label = 16; break; }
    case 13: 
      var $86=$t;
      $v=$86;
      var $87=$trem;
      $rsize=$87;
      var $88=(($87)|(0))==0;
      if ($88) { label = 14; break; } else { label = 15; break; }
    case 14: 
      label = 22; break;
    case 15: 
      label = 16; break;
    case 16: 
      var $92=$t;
      var $93=(($92+16)|0);
      var $94=(($93+4)|0);
      var $95=HEAP32[(($94)>>2)];
      $rt=$95;
      var $96=$sizebits;
      var $97=$96 >>> 31;
      var $98=$97 & 1;
      var $99=$t;
      var $100=(($99+16)|0);
      var $101=(($100+($98<<2))|0);
      var $102=HEAP32[(($101)>>2)];
      $t=$102;
      var $103=$rt;
      var $104=(($103)|(0))!=0;
      if ($104) { label = 17; break; } else { label = 19; break; }
    case 17: 
      var $106=$rt;
      var $107=$t;
      var $108=(($106)|(0))!=(($107)|(0));
      if ($108) { label = 18; break; } else { label = 19; break; }
    case 18: 
      var $110=$rt;
      $rst=$110;
      label = 19; break;
    case 19: 
      var $112=$t;
      var $113=(($112)|(0))==0;
      if ($113) { label = 20; break; } else { label = 21; break; }
    case 20: 
      var $115=$rst;
      $t=$115;
      label = 22; break;
    case 21: 
      var $117=$sizebits;
      var $118=$117 << 1;
      $sizebits=$118;
      label = 12; break;
    case 22: 
      label = 23; break;
    case 23: 
      var $121=$t;
      var $122=(($121)|(0))==0;
      if ($122) { label = 24; break; } else { label = 28; break; }
    case 24: 
      var $124=$v;
      var $125=(($124)|(0))==0;
      if ($125) { label = 25; break; } else { label = 28; break; }
    case 25: 
      var $127=$idx;
      var $128=1 << $127;
      var $129=$128 << 1;
      var $130=$idx;
      var $131=1 << $130;
      var $132=$131 << 1;
      var $133=(((-$132))|0);
      var $134=$129 | $133;
      var $135=$2;
      var $136=(($135+4)|0);
      var $137=HEAP32[(($136)>>2)];
      var $138=$134 & $137;
      $leftbits=$138;
      var $139=$leftbits;
      var $140=(($139)|(0))!=0;
      if ($140) { label = 26; break; } else { label = 27; break; }
    case 26: 
      var $142=$leftbits;
      var $143=$leftbits;
      var $144=(((-$143))|0);
      var $145=$142 & $144;
      $leastbit=$145;
      var $146=$leastbit;
      var $147=((($146)-(1))|0);
      $Y1=$147;
      var $148=$Y1;
      var $149=$148 >>> 12;
      var $150=$149 & 16;
      $K2=$150;
      var $151=$K2;
      $N3=$151;
      var $152=$K2;
      var $153=$Y1;
      var $154=$153 >>> (($152)>>>(0));
      $Y1=$154;
      var $155=$Y1;
      var $156=$155 >>> 5;
      var $157=$156 & 8;
      $K2=$157;
      var $158=$N3;
      var $159=((($158)+($157))|0);
      $N3=$159;
      var $160=$K2;
      var $161=$Y1;
      var $162=$161 >>> (($160)>>>(0));
      $Y1=$162;
      var $163=$Y1;
      var $164=$163 >>> 2;
      var $165=$164 & 4;
      $K2=$165;
      var $166=$N3;
      var $167=((($166)+($165))|0);
      $N3=$167;
      var $168=$K2;
      var $169=$Y1;
      var $170=$169 >>> (($168)>>>(0));
      $Y1=$170;
      var $171=$Y1;
      var $172=$171 >>> 1;
      var $173=$172 & 2;
      $K2=$173;
      var $174=$N3;
      var $175=((($174)+($173))|0);
      $N3=$175;
      var $176=$K2;
      var $177=$Y1;
      var $178=$177 >>> (($176)>>>(0));
      $Y1=$178;
      var $179=$Y1;
      var $180=$179 >>> 1;
      var $181=$180 & 1;
      $K2=$181;
      var $182=$N3;
      var $183=((($182)+($181))|0);
      $N3=$183;
      var $184=$K2;
      var $185=$Y1;
      var $186=$185 >>> (($184)>>>(0));
      $Y1=$186;
      var $187=$N3;
      var $188=$Y1;
      var $189=((($187)+($188))|0);
      $i=$189;
      var $190=$i;
      var $191=$2;
      var $192=(($191+304)|0);
      var $193=(($192+($190<<2))|0);
      var $194=HEAP32[(($193)>>2)];
      $t=$194;
      label = 27; break;
    case 27: 
      label = 28; break;
    case 28: 
      label = 29; break;
    case 29: 
      var $198=$t;
      var $199=(($198)|(0))!=0;
      if ($199) { label = 30; break; } else { label = 36; break; }
    case 30: 
      var $201=$t;
      var $202=(($201+4)|0);
      var $203=HEAP32[(($202)>>2)];
      var $204=$203 & -8;
      var $205=$3;
      var $206=((($204)-($205))|0);
      $trem4=$206;
      var $207=$trem4;
      var $208=$rsize;
      var $209=(($207)>>>(0)) < (($208)>>>(0));
      if ($209) { label = 31; break; } else { label = 32; break; }
    case 31: 
      var $211=$trem4;
      $rsize=$211;
      var $212=$t;
      $v=$212;
      label = 32; break;
    case 32: 
      var $214=$t;
      var $215=(($214+16)|0);
      var $216=(($215)|0);
      var $217=HEAP32[(($216)>>2)];
      var $218=(($217)|(0))!=0;
      if ($218) { label = 33; break; } else { label = 34; break; }
    case 33: 
      var $220=$t;
      var $221=(($220+16)|0);
      var $222=(($221)|0);
      var $223=HEAP32[(($222)>>2)];
      var $230 = $223;label = 35; break;
    case 34: 
      var $225=$t;
      var $226=(($225+16)|0);
      var $227=(($226+4)|0);
      var $228=HEAP32[(($227)>>2)];
      var $230 = $228;label = 35; break;
    case 35: 
      var $230;
      $t=$230;
      label = 29; break;
    case 36: 
      var $232=$v;
      var $233=(($232)|(0))!=0;
      if ($233) { label = 37; break; } else { label = 129; break; }
    case 37: 
      var $235=$rsize;
      var $236=$2;
      var $237=(($236+8)|0);
      var $238=HEAP32[(($237)>>2)];
      var $239=$3;
      var $240=((($238)-($239))|0);
      var $241=(($235)>>>(0)) < (($240)>>>(0));
      if ($241) { label = 38; break; } else { label = 129; break; }
    case 38: 
      var $243=$v;
      var $244=$243;
      var $245=$2;
      var $246=(($245+16)|0);
      var $247=HEAP32[(($246)>>2)];
      var $248=(($244)>>>(0)) >= (($247)>>>(0));
      var $249=(($248)&(1));
      var $250=($249);
      var $251=(($250)|(0))!=0;
      if ($251) { label = 39; break; } else { label = 128; break; }
    case 39: 
      var $253=$v;
      var $254=$253;
      var $255=$3;
      var $256=(($254+$255)|0);
      var $257=$256;
      $r=$257;
      var $258=$v;
      var $259=$258;
      var $260=$r;
      var $261=$260;
      var $262=(($259)>>>(0)) < (($261)>>>(0));
      var $263=(($262)&(1));
      var $264=($263);
      var $265=(($264)|(0))!=0;
      if ($265) { label = 40; break; } else { label = 127; break; }
    case 40: 
      var $267=$v;
      var $268=(($267+24)|0);
      var $269=HEAP32[(($268)>>2)];
      $XP=$269;
      var $270=$v;
      var $271=(($270+12)|0);
      var $272=HEAP32[(($271)>>2)];
      var $273=$v;
      var $274=(($272)|(0))!=(($273)|(0));
      if ($274) { label = 41; break; } else { label = 48; break; }
    case 41: 
      var $276=$v;
      var $277=(($276+8)|0);
      var $278=HEAP32[(($277)>>2)];
      $F=$278;
      var $279=$v;
      var $280=(($279+12)|0);
      var $281=HEAP32[(($280)>>2)];
      $R=$281;
      var $282=$F;
      var $283=$282;
      var $284=$2;
      var $285=(($284+16)|0);
      var $286=HEAP32[(($285)>>2)];
      var $287=(($283)>>>(0)) >= (($286)>>>(0));
      if ($287) { label = 42; break; } else { var $301 = 0;label = 44; break; }
    case 42: 
      var $289=$F;
      var $290=(($289+12)|0);
      var $291=HEAP32[(($290)>>2)];
      var $292=$v;
      var $293=(($291)|(0))==(($292)|(0));
      if ($293) { label = 43; break; } else { var $301 = 0;label = 44; break; }
    case 43: 
      var $295=$R;
      var $296=(($295+8)|0);
      var $297=HEAP32[(($296)>>2)];
      var $298=$v;
      var $299=(($297)|(0))==(($298)|(0));
      var $301 = $299;label = 44; break;
    case 44: 
      var $301;
      var $302=(($301)&(1));
      var $303=($302);
      var $304=(($303)|(0))!=0;
      if ($304) { label = 45; break; } else { label = 46; break; }
    case 45: 
      var $306=$R;
      var $307=$F;
      var $308=(($307+12)|0);
      HEAP32[(($308)>>2)]=$306;
      var $309=$F;
      var $310=$R;
      var $311=(($310+8)|0);
      HEAP32[(($311)>>2)]=$309;
      label = 47; break;
    case 46: 
      _abort();
      throw "Reached an unreachable!";
    case 47: 
      label = 60; break;
    case 48: 
      var $315=$v;
      var $316=(($315+16)|0);
      var $317=(($316+4)|0);
      $RP=$317;
      var $318=HEAP32[(($317)>>2)];
      $R=$318;
      var $319=(($318)|(0))!=0;
      if ($319) { label = 50; break; } else { label = 49; break; }
    case 49: 
      var $321=$v;
      var $322=(($321+16)|0);
      var $323=(($322)|0);
      $RP=$323;
      var $324=HEAP32[(($323)>>2)];
      $R=$324;
      var $325=(($324)|(0))!=0;
      if ($325) { label = 50; break; } else { label = 59; break; }
    case 50: 
      label = 51; break;
    case 51: 
      var $328=$R;
      var $329=(($328+16)|0);
      var $330=(($329+4)|0);
      $CP=$330;
      var $331=HEAP32[(($330)>>2)];
      var $332=(($331)|(0))!=0;
      if ($332) { var $340 = 1;label = 53; break; } else { label = 52; break; }
    case 52: 
      var $334=$R;
      var $335=(($334+16)|0);
      var $336=(($335)|0);
      $CP=$336;
      var $337=HEAP32[(($336)>>2)];
      var $338=(($337)|(0))!=0;
      var $340 = $338;label = 53; break;
    case 53: 
      var $340;
      if ($340) { label = 54; break; } else { label = 55; break; }
    case 54: 
      var $342=$CP;
      $RP=$342;
      var $343=HEAP32[(($342)>>2)];
      $R=$343;
      label = 51; break;
    case 55: 
      var $345=$RP;
      var $346=$345;
      var $347=$2;
      var $348=(($347+16)|0);
      var $349=HEAP32[(($348)>>2)];
      var $350=(($346)>>>(0)) >= (($349)>>>(0));
      var $351=(($350)&(1));
      var $352=($351);
      var $353=(($352)|(0))!=0;
      if ($353) { label = 56; break; } else { label = 57; break; }
    case 56: 
      var $355=$RP;
      HEAP32[(($355)>>2)]=0;
      label = 58; break;
    case 57: 
      _abort();
      throw "Reached an unreachable!";
    case 58: 
      label = 59; break;
    case 59: 
      label = 60; break;
    case 60: 
      var $360=$XP;
      var $361=(($360)|(0))!=0;
      if ($361) { label = 61; break; } else { label = 88; break; }
    case 61: 
      var $363=$v;
      var $364=(($363+28)|0);
      var $365=HEAP32[(($364)>>2)];
      var $366=$2;
      var $367=(($366+304)|0);
      var $368=(($367+($365<<2))|0);
      $H=$368;
      var $369=$v;
      var $370=$H;
      var $371=HEAP32[(($370)>>2)];
      var $372=(($369)|(0))==(($371)|(0));
      if ($372) { label = 62; break; } else { label = 65; break; }
    case 62: 
      var $374=$R;
      var $375=$H;
      HEAP32[(($375)>>2)]=$374;
      var $376=(($374)|(0))==0;
      if ($376) { label = 63; break; } else { label = 64; break; }
    case 63: 
      var $378=$v;
      var $379=(($378+28)|0);
      var $380=HEAP32[(($379)>>2)];
      var $381=1 << $380;
      var $382=$381 ^ -1;
      var $383=$2;
      var $384=(($383+4)|0);
      var $385=HEAP32[(($384)>>2)];
      var $386=$385 & $382;
      HEAP32[(($384)>>2)]=$386;
      label = 64; break;
    case 64: 
      label = 72; break;
    case 65: 
      var $389=$XP;
      var $390=$389;
      var $391=$2;
      var $392=(($391+16)|0);
      var $393=HEAP32[(($392)>>2)];
      var $394=(($390)>>>(0)) >= (($393)>>>(0));
      var $395=(($394)&(1));
      var $396=($395);
      var $397=(($396)|(0))!=0;
      if ($397) { label = 66; break; } else { label = 70; break; }
    case 66: 
      var $399=$XP;
      var $400=(($399+16)|0);
      var $401=(($400)|0);
      var $402=HEAP32[(($401)>>2)];
      var $403=$v;
      var $404=(($402)|(0))==(($403)|(0));
      if ($404) { label = 67; break; } else { label = 68; break; }
    case 67: 
      var $406=$R;
      var $407=$XP;
      var $408=(($407+16)|0);
      var $409=(($408)|0);
      HEAP32[(($409)>>2)]=$406;
      label = 69; break;
    case 68: 
      var $411=$R;
      var $412=$XP;
      var $413=(($412+16)|0);
      var $414=(($413+4)|0);
      HEAP32[(($414)>>2)]=$411;
      label = 69; break;
    case 69: 
      label = 71; break;
    case 70: 
      _abort();
      throw "Reached an unreachable!";
    case 71: 
      label = 72; break;
    case 72: 
      var $419=$R;
      var $420=(($419)|(0))!=0;
      if ($420) { label = 73; break; } else { label = 87; break; }
    case 73: 
      var $422=$R;
      var $423=$422;
      var $424=$2;
      var $425=(($424+16)|0);
      var $426=HEAP32[(($425)>>2)];
      var $427=(($423)>>>(0)) >= (($426)>>>(0));
      var $428=(($427)&(1));
      var $429=($428);
      var $430=(($429)|(0))!=0;
      if ($430) { label = 74; break; } else { label = 85; break; }
    case 74: 
      var $432=$XP;
      var $433=$R;
      var $434=(($433+24)|0);
      HEAP32[(($434)>>2)]=$432;
      var $435=$v;
      var $436=(($435+16)|0);
      var $437=(($436)|0);
      var $438=HEAP32[(($437)>>2)];
      $C0=$438;
      var $439=(($438)|(0))!=0;
      if ($439) { label = 75; break; } else { label = 79; break; }
    case 75: 
      var $441=$C0;
      var $442=$441;
      var $443=$2;
      var $444=(($443+16)|0);
      var $445=HEAP32[(($444)>>2)];
      var $446=(($442)>>>(0)) >= (($445)>>>(0));
      var $447=(($446)&(1));
      var $448=($447);
      var $449=(($448)|(0))!=0;
      if ($449) { label = 76; break; } else { label = 77; break; }
    case 76: 
      var $451=$C0;
      var $452=$R;
      var $453=(($452+16)|0);
      var $454=(($453)|0);
      HEAP32[(($454)>>2)]=$451;
      var $455=$R;
      var $456=$C0;
      var $457=(($456+24)|0);
      HEAP32[(($457)>>2)]=$455;
      label = 78; break;
    case 77: 
      _abort();
      throw "Reached an unreachable!";
    case 78: 
      label = 79; break;
    case 79: 
      var $461=$v;
      var $462=(($461+16)|0);
      var $463=(($462+4)|0);
      var $464=HEAP32[(($463)>>2)];
      $C1=$464;
      var $465=(($464)|(0))!=0;
      if ($465) { label = 80; break; } else { label = 84; break; }
    case 80: 
      var $467=$C1;
      var $468=$467;
      var $469=$2;
      var $470=(($469+16)|0);
      var $471=HEAP32[(($470)>>2)];
      var $472=(($468)>>>(0)) >= (($471)>>>(0));
      var $473=(($472)&(1));
      var $474=($473);
      var $475=(($474)|(0))!=0;
      if ($475) { label = 81; break; } else { label = 82; break; }
    case 81: 
      var $477=$C1;
      var $478=$R;
      var $479=(($478+16)|0);
      var $480=(($479+4)|0);
      HEAP32[(($480)>>2)]=$477;
      var $481=$R;
      var $482=$C1;
      var $483=(($482+24)|0);
      HEAP32[(($483)>>2)]=$481;
      label = 83; break;
    case 82: 
      _abort();
      throw "Reached an unreachable!";
    case 83: 
      label = 84; break;
    case 84: 
      label = 86; break;
    case 85: 
      _abort();
      throw "Reached an unreachable!";
    case 86: 
      label = 87; break;
    case 87: 
      label = 88; break;
    case 88: 
      var $491=$rsize;
      var $492=(($491)>>>(0)) < 16;
      if ($492) { label = 89; break; } else { label = 90; break; }
    case 89: 
      var $494=$rsize;
      var $495=$3;
      var $496=((($494)+($495))|0);
      var $497=$496 | 1;
      var $498=$497 | 2;
      var $499=$v;
      var $500=(($499+4)|0);
      HEAP32[(($500)>>2)]=$498;
      var $501=$v;
      var $502=$501;
      var $503=$rsize;
      var $504=$3;
      var $505=((($503)+($504))|0);
      var $506=(($502+$505)|0);
      var $507=$506;
      var $508=(($507+4)|0);
      var $509=HEAP32[(($508)>>2)];
      var $510=$509 | 1;
      HEAP32[(($508)>>2)]=$510;
      label = 126; break;
    case 90: 
      var $512=$3;
      var $513=$512 | 1;
      var $514=$513 | 2;
      var $515=$v;
      var $516=(($515+4)|0);
      HEAP32[(($516)>>2)]=$514;
      var $517=$rsize;
      var $518=$517 | 1;
      var $519=$r;
      var $520=(($519+4)|0);
      HEAP32[(($520)>>2)]=$518;
      var $521=$rsize;
      var $522=$r;
      var $523=$522;
      var $524=$rsize;
      var $525=(($523+$524)|0);
      var $526=$525;
      var $527=(($526)|0);
      HEAP32[(($527)>>2)]=$521;
      var $528=$rsize;
      var $529=$528 >>> 3;
      var $530=(($529)>>>(0)) < 32;
      if ($530) { label = 91; break; } else { label = 98; break; }
    case 91: 
      var $532=$rsize;
      var $533=$532 >>> 3;
      $I=$533;
      var $534=$I;
      var $535=$534 << 1;
      var $536=$2;
      var $537=(($536+40)|0);
      var $538=(($537+($535<<2))|0);
      var $539=$538;
      var $540=$539;
      $B=$540;
      var $541=$B;
      $F5=$541;
      var $542=$2;
      var $543=(($542)|0);
      var $544=HEAP32[(($543)>>2)];
      var $545=$I;
      var $546=1 << $545;
      var $547=$544 & $546;
      var $548=(($547)|(0))!=0;
      if ($548) { label = 93; break; } else { label = 92; break; }
    case 92: 
      var $550=$I;
      var $551=1 << $550;
      var $552=$2;
      var $553=(($552)|0);
      var $554=HEAP32[(($553)>>2)];
      var $555=$554 | $551;
      HEAP32[(($553)>>2)]=$555;
      label = 97; break;
    case 93: 
      var $557=$B;
      var $558=(($557+8)|0);
      var $559=HEAP32[(($558)>>2)];
      var $560=$559;
      var $561=$2;
      var $562=(($561+16)|0);
      var $563=HEAP32[(($562)>>2)];
      var $564=(($560)>>>(0)) >= (($563)>>>(0));
      var $565=(($564)&(1));
      var $566=($565);
      var $567=(($566)|(0))!=0;
      if ($567) { label = 94; break; } else { label = 95; break; }
    case 94: 
      var $569=$B;
      var $570=(($569+8)|0);
      var $571=HEAP32[(($570)>>2)];
      $F5=$571;
      label = 96; break;
    case 95: 
      _abort();
      throw "Reached an unreachable!";
    case 96: 
      label = 97; break;
    case 97: 
      var $575=$r;
      var $576=$B;
      var $577=(($576+8)|0);
      HEAP32[(($577)>>2)]=$575;
      var $578=$r;
      var $579=$F5;
      var $580=(($579+12)|0);
      HEAP32[(($580)>>2)]=$578;
      var $581=$F5;
      var $582=$r;
      var $583=(($582+8)|0);
      HEAP32[(($583)>>2)]=$581;
      var $584=$B;
      var $585=$r;
      var $586=(($585+12)|0);
      HEAP32[(($586)>>2)]=$584;
      label = 125; break;
    case 98: 
      var $588=$r;
      var $589=$588;
      $TP=$589;
      var $590=$rsize;
      var $591=$590 >>> 8;
      $X8=$591;
      var $592=$X8;
      var $593=(($592)|(0))==0;
      if ($593) { label = 99; break; } else { label = 100; break; }
    case 99: 
      $I7=0;
      label = 104; break;
    case 100: 
      var $596=$X8;
      var $597=(($596)>>>(0)) > 65535;
      if ($597) { label = 101; break; } else { label = 102; break; }
    case 101: 
      $I7=31;
      label = 103; break;
    case 102: 
      var $600=$X8;
      $Y9=$600;
      var $601=$Y9;
      var $602=((($601)-(256))|0);
      var $603=$602 >>> 16;
      var $604=$603 & 8;
      $N10=$604;
      var $605=$N10;
      var $606=$Y9;
      var $607=$606 << $605;
      $Y9=$607;
      var $608=((($607)-(4096))|0);
      var $609=$608 >>> 16;
      var $610=$609 & 4;
      $K11=$610;
      var $611=$K11;
      var $612=$N10;
      var $613=((($612)+($611))|0);
      $N10=$613;
      var $614=$K11;
      var $615=$Y9;
      var $616=$615 << $614;
      $Y9=$616;
      var $617=((($616)-(16384))|0);
      var $618=$617 >>> 16;
      var $619=$618 & 2;
      $K11=$619;
      var $620=$N10;
      var $621=((($620)+($619))|0);
      $N10=$621;
      var $622=$N10;
      var $623=(((14)-($622))|0);
      var $624=$K11;
      var $625=$Y9;
      var $626=$625 << $624;
      $Y9=$626;
      var $627=$626 >>> 15;
      var $628=((($623)+($627))|0);
      $K11=$628;
      var $629=$K11;
      var $630=$629 << 1;
      var $631=$rsize;
      var $632=$K11;
      var $633=((($632)+(7))|0);
      var $634=$631 >>> (($633)>>>(0));
      var $635=$634 & 1;
      var $636=((($630)+($635))|0);
      $I7=$636;
      label = 103; break;
    case 103: 
      label = 104; break;
    case 104: 
      var $639=$I7;
      var $640=$2;
      var $641=(($640+304)|0);
      var $642=(($641+($639<<2))|0);
      $H6=$642;
      var $643=$I7;
      var $644=$TP;
      var $645=(($644+28)|0);
      HEAP32[(($645)>>2)]=$643;
      var $646=$TP;
      var $647=(($646+16)|0);
      var $648=(($647+4)|0);
      HEAP32[(($648)>>2)]=0;
      var $649=$TP;
      var $650=(($649+16)|0);
      var $651=(($650)|0);
      HEAP32[(($651)>>2)]=0;
      var $652=$2;
      var $653=(($652+4)|0);
      var $654=HEAP32[(($653)>>2)];
      var $655=$I7;
      var $656=1 << $655;
      var $657=$654 & $656;
      var $658=(($657)|(0))!=0;
      if ($658) { label = 106; break; } else { label = 105; break; }
    case 105: 
      var $660=$I7;
      var $661=1 << $660;
      var $662=$2;
      var $663=(($662+4)|0);
      var $664=HEAP32[(($663)>>2)];
      var $665=$664 | $661;
      HEAP32[(($663)>>2)]=$665;
      var $666=$TP;
      var $667=$H6;
      HEAP32[(($667)>>2)]=$666;
      var $668=$H6;
      var $669=$668;
      var $670=$TP;
      var $671=(($670+24)|0);
      HEAP32[(($671)>>2)]=$669;
      var $672=$TP;
      var $673=$TP;
      var $674=(($673+12)|0);
      HEAP32[(($674)>>2)]=$672;
      var $675=$TP;
      var $676=(($675+8)|0);
      HEAP32[(($676)>>2)]=$672;
      label = 124; break;
    case 106: 
      var $678=$H6;
      var $679=HEAP32[(($678)>>2)];
      $T=$679;
      var $680=$rsize;
      var $681=$I7;
      var $682=(($681)|(0))==31;
      if ($682) { label = 107; break; } else { label = 108; break; }
    case 107: 
      var $691 = 0;label = 109; break;
    case 108: 
      var $685=$I7;
      var $686=$685 >>> 1;
      var $687=((($686)+(8))|0);
      var $688=((($687)-(2))|0);
      var $689=(((31)-($688))|0);
      var $691 = $689;label = 109; break;
    case 109: 
      var $691;
      var $692=$680 << $691;
      $K12=$692;
      label = 110; break;
    case 110: 
      var $694=$T;
      var $695=(($694+4)|0);
      var $696=HEAP32[(($695)>>2)];
      var $697=$696 & -8;
      var $698=$rsize;
      var $699=(($697)|(0))!=(($698)|(0));
      if ($699) { label = 111; break; } else { label = 117; break; }
    case 111: 
      var $701=$K12;
      var $702=$701 >>> 31;
      var $703=$702 & 1;
      var $704=$T;
      var $705=(($704+16)|0);
      var $706=(($705+($703<<2))|0);
      $C=$706;
      var $707=$K12;
      var $708=$707 << 1;
      $K12=$708;
      var $709=$C;
      var $710=HEAP32[(($709)>>2)];
      var $711=(($710)|(0))!=0;
      if ($711) { label = 112; break; } else { label = 113; break; }
    case 112: 
      var $713=$C;
      var $714=HEAP32[(($713)>>2)];
      $T=$714;
      label = 116; break;
    case 113: 
      var $716=$C;
      var $717=$716;
      var $718=$2;
      var $719=(($718+16)|0);
      var $720=HEAP32[(($719)>>2)];
      var $721=(($717)>>>(0)) >= (($720)>>>(0));
      var $722=(($721)&(1));
      var $723=($722);
      var $724=(($723)|(0))!=0;
      if ($724) { label = 114; break; } else { label = 115; break; }
    case 114: 
      var $726=$TP;
      var $727=$C;
      HEAP32[(($727)>>2)]=$726;
      var $728=$T;
      var $729=$TP;
      var $730=(($729+24)|0);
      HEAP32[(($730)>>2)]=$728;
      var $731=$TP;
      var $732=$TP;
      var $733=(($732+12)|0);
      HEAP32[(($733)>>2)]=$731;
      var $734=$TP;
      var $735=(($734+8)|0);
      HEAP32[(($735)>>2)]=$731;
      label = 123; break;
    case 115: 
      _abort();
      throw "Reached an unreachable!";
    case 116: 
      label = 122; break;
    case 117: 
      var $739=$T;
      var $740=(($739+8)|0);
      var $741=HEAP32[(($740)>>2)];
      $F13=$741;
      var $742=$T;
      var $743=$742;
      var $744=$2;
      var $745=(($744+16)|0);
      var $746=HEAP32[(($745)>>2)];
      var $747=(($743)>>>(0)) >= (($746)>>>(0));
      if ($747) { label = 118; break; } else { var $756 = 0;label = 119; break; }
    case 118: 
      var $749=$F13;
      var $750=$749;
      var $751=$2;
      var $752=(($751+16)|0);
      var $753=HEAP32[(($752)>>2)];
      var $754=(($750)>>>(0)) >= (($753)>>>(0));
      var $756 = $754;label = 119; break;
    case 119: 
      var $756;
      var $757=(($756)&(1));
      var $758=($757);
      var $759=(($758)|(0))!=0;
      if ($759) { label = 120; break; } else { label = 121; break; }
    case 120: 
      var $761=$TP;
      var $762=$F13;
      var $763=(($762+12)|0);
      HEAP32[(($763)>>2)]=$761;
      var $764=$T;
      var $765=(($764+8)|0);
      HEAP32[(($765)>>2)]=$761;
      var $766=$F13;
      var $767=$TP;
      var $768=(($767+8)|0);
      HEAP32[(($768)>>2)]=$766;
      var $769=$T;
      var $770=$TP;
      var $771=(($770+12)|0);
      HEAP32[(($771)>>2)]=$769;
      var $772=$TP;
      var $773=(($772+24)|0);
      HEAP32[(($773)>>2)]=0;
      label = 123; break;
    case 121: 
      _abort();
      throw "Reached an unreachable!";
    case 122: 
      label = 110; break;
    case 123: 
      label = 124; break;
    case 124: 
      label = 125; break;
    case 125: 
      label = 126; break;
    case 126: 
      var $780=$v;
      var $781=$780;
      var $782=(($781+8)|0);
      $1=$782;
      label = 130; break;
    case 127: 
      label = 128; break;
    case 128: 
      _abort();
      throw "Reached an unreachable!";
    case 129: 
      $1=0;
      label = 130; break;
    case 130: 
      var $787=$1;
      return $787;
    default: assert(0, "bad label: " + label);
  }
}
function _sys_alloc($m, $nb) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $tbase;
      var $tsize;
      var $mmap_flag;
      var $asize;
      var $mem;
      var $fp;
      var $br;
      var $ssize;
      var $ss;
      var $base;
      var $fp1;
      var $esize;
      var $end;
      var $br2;
      var $end3;
      var $ssize4;
      var $mn;
      var $sp;
      var $oldbase;
      var $rsize;
      var $p;
      var $r;
      $2=$m;
      $3=$nb;
      $tbase=-1;
      $tsize=0;
      $mmap_flag=0;
      var $4=HEAP32[((((56)|0))>>2)];
      var $5=(($4)|(0))!=0;
      if ($5) { var $10 = 1;label = 3; break; } else { label = 2; break; }
    case 2: 
      var $7=_init_mparams();
      var $8=(($7)|(0))!=0;
      var $10 = $8;label = 3; break;
    case 3: 
      var $10;
      var $11=(($10)&(1));
      var $12=$2;
      var $13=(($12+444)|0);
      var $14=HEAP32[(($13)>>2)];
      var $15=$14 & 0;
      var $16=(($15)|(0))!=0;
      if ($16) { label = 4; break; } else { label = 9; break; }
    case 4: 
      var $18=$3;
      var $19=HEAP32[((((68)|0))>>2)];
      var $20=(($18)>>>(0)) >= (($19)>>>(0));
      if ($20) { label = 5; break; } else { label = 9; break; }
    case 5: 
      var $22=$2;
      var $23=(($22+12)|0);
      var $24=HEAP32[(($23)>>2)];
      var $25=(($24)|(0))!=0;
      if ($25) { label = 6; break; } else { label = 9; break; }
    case 6: 
      var $27=$2;
      var $28=$3;
      var $29=_mmap_alloc($27, $28);
      $mem=$29;
      var $30=$mem;
      var $31=(($30)|(0))!=0;
      if ($31) { label = 7; break; } else { label = 8; break; }
    case 7: 
      var $33=$mem;
      $1=$33;
      label = 103; break;
    case 8: 
      label = 9; break;
    case 9: 
      var $36=$3;
      var $37=((($36)+(48))|0);
      var $38=HEAP32[((((64)|0))>>2)];
      var $39=((($38)-(1))|0);
      var $40=((($37)+($39))|0);
      var $41=HEAP32[((((64)|0))>>2)];
      var $42=((($41)-(1))|0);
      var $43=$42 ^ -1;
      var $44=$40 & $43;
      $asize=$44;
      var $45=$asize;
      var $46=$3;
      var $47=(($45)>>>(0)) <= (($46)>>>(0));
      if ($47) { label = 10; break; } else { label = 11; break; }
    case 10: 
      $1=0;
      label = 103; break;
    case 11: 
      var $50=$2;
      var $51=(($50+440)|0);
      var $52=HEAP32[(($51)>>2)];
      var $53=(($52)|(0))!=0;
      if ($53) { label = 12; break; } else { label = 16; break; }
    case 12: 
      var $55=$2;
      var $56=(($55+432)|0);
      var $57=HEAP32[(($56)>>2)];
      var $58=$asize;
      var $59=((($57)+($58))|0);
      $fp=$59;
      var $60=$fp;
      var $61=$2;
      var $62=(($61+432)|0);
      var $63=HEAP32[(($62)>>2)];
      var $64=(($60)>>>(0)) <= (($63)>>>(0));
      if ($64) { label = 14; break; } else { label = 13; break; }
    case 13: 
      var $66=$fp;
      var $67=$2;
      var $68=(($67+440)|0);
      var $69=HEAP32[(($68)>>2)];
      var $70=(($66)>>>(0)) > (($69)>>>(0));
      if ($70) { label = 14; break; } else { label = 15; break; }
    case 14: 
      $1=0;
      label = 103; break;
    case 15: 
      label = 16; break;
    case 16: 
      var $74=$2;
      var $75=(($74+444)|0);
      var $76=HEAP32[(($75)>>2)];
      var $77=$76 & 4;
      var $78=(($77)|(0))!=0;
      if ($78) { label = 53; break; } else { label = 17; break; }
    case 17: 
      $br=-1;
      var $80=$asize;
      $ssize=$80;
      var $81=$2;
      var $82=(($81+24)|0);
      var $83=HEAP32[(($82)>>2)];
      var $84=(($83)|(0))==0;
      if ($84) { label = 18; break; } else { label = 19; break; }
    case 18: 
      var $94 = 0;label = 20; break;
    case 19: 
      var $87=$2;
      var $88=$2;
      var $89=(($88+24)|0);
      var $90=HEAP32[(($89)>>2)];
      var $91=$90;
      var $92=_segment_holding($87, $91);
      var $94 = $92;label = 20; break;
    case 20: 
      var $94;
      $ss=$94;
      var $95=$ss;
      var $96=(($95)|(0))==0;
      if ($96) { label = 21; break; } else { label = 33; break; }
    case 21: 
      var $98=_sbrk(0);
      $base=$98;
      var $99=$base;
      var $100=(($99)|(0))!=-1;
      if ($100) { label = 22; break; } else { label = 32; break; }
    case 22: 
      var $102=$base;
      var $103=$102;
      var $104=HEAP32[((((60)|0))>>2)];
      var $105=((($104)-(1))|0);
      var $106=$103 & $105;
      var $107=(($106)|(0))==0;
      if ($107) { label = 24; break; } else { label = 23; break; }
    case 23: 
      var $109=$base;
      var $110=$109;
      var $111=HEAP32[((((60)|0))>>2)];
      var $112=((($111)-(1))|0);
      var $113=((($110)+($112))|0);
      var $114=HEAP32[((((60)|0))>>2)];
      var $115=((($114)-(1))|0);
      var $116=$115 ^ -1;
      var $117=$113 & $116;
      var $118=$base;
      var $119=$118;
      var $120=((($117)-($119))|0);
      var $121=$ssize;
      var $122=((($121)+($120))|0);
      $ssize=$122;
      label = 24; break;
    case 24: 
      var $124=$2;
      var $125=(($124+432)|0);
      var $126=HEAP32[(($125)>>2)];
      var $127=$ssize;
      var $128=((($126)+($127))|0);
      $fp1=$128;
      var $129=$ssize;
      var $130=$3;
      var $131=(($129)>>>(0)) > (($130)>>>(0));
      if ($131) { label = 25; break; } else { label = 31; break; }
    case 25: 
      var $133=$ssize;
      var $134=(($133)>>>(0)) < 2147483647;
      if ($134) { label = 26; break; } else { label = 31; break; }
    case 26: 
      var $136=$2;
      var $137=(($136+440)|0);
      var $138=HEAP32[(($137)>>2)];
      var $139=(($138)|(0))==0;
      if ($139) { label = 29; break; } else { label = 27; break; }
    case 27: 
      var $141=$fp1;
      var $142=$2;
      var $143=(($142+432)|0);
      var $144=HEAP32[(($143)>>2)];
      var $145=(($141)>>>(0)) > (($144)>>>(0));
      if ($145) { label = 28; break; } else { label = 31; break; }
    case 28: 
      var $147=$fp1;
      var $148=$2;
      var $149=(($148+440)|0);
      var $150=HEAP32[(($149)>>2)];
      var $151=(($147)>>>(0)) <= (($150)>>>(0));
      if ($151) { label = 29; break; } else { label = 31; break; }
    case 29: 
      var $153=$ssize;
      var $154=_sbrk($153);
      $br=$154;
      var $155=$base;
      var $156=(($154)|(0))==(($155)|(0));
      if ($156) { label = 30; break; } else { label = 31; break; }
    case 30: 
      var $158=$base;
      $tbase=$158;
      var $159=$ssize;
      $tsize=$159;
      label = 31; break;
    case 31: 
      label = 32; break;
    case 32: 
      label = 37; break;
    case 33: 
      var $163=$3;
      var $164=$2;
      var $165=(($164+12)|0);
      var $166=HEAP32[(($165)>>2)];
      var $167=((($163)-($166))|0);
      var $168=((($167)+(48))|0);
      var $169=HEAP32[((((64)|0))>>2)];
      var $170=((($169)-(1))|0);
      var $171=((($168)+($170))|0);
      var $172=HEAP32[((((64)|0))>>2)];
      var $173=((($172)-(1))|0);
      var $174=$173 ^ -1;
      var $175=$171 & $174;
      $ssize=$175;
      var $176=$ssize;
      var $177=(($176)>>>(0)) < 2147483647;
      if ($177) { label = 34; break; } else { label = 36; break; }
    case 34: 
      var $179=$ssize;
      var $180=_sbrk($179);
      $br=$180;
      var $181=$ss;
      var $182=(($181)|0);
      var $183=HEAP32[(($182)>>2)];
      var $184=$ss;
      var $185=(($184+4)|0);
      var $186=HEAP32[(($185)>>2)];
      var $187=(($183+$186)|0);
      var $188=(($180)|(0))==(($187)|(0));
      if ($188) { label = 35; break; } else { label = 36; break; }
    case 35: 
      var $190=$br;
      $tbase=$190;
      var $191=$ssize;
      $tsize=$191;
      label = 36; break;
    case 36: 
      label = 37; break;
    case 37: 
      var $194=$tbase;
      var $195=(($194)|(0))==-1;
      if ($195) { label = 38; break; } else { label = 52; break; }
    case 38: 
      var $197=$br;
      var $198=(($197)|(0))!=-1;
      if ($198) { label = 39; break; } else { label = 48; break; }
    case 39: 
      var $200=$ssize;
      var $201=(($200)>>>(0)) < 2147483647;
      if ($201) { label = 40; break; } else { label = 47; break; }
    case 40: 
      var $203=$ssize;
      var $204=$3;
      var $205=((($204)+(48))|0);
      var $206=(($203)>>>(0)) < (($205)>>>(0));
      if ($206) { label = 41; break; } else { label = 47; break; }
    case 41: 
      var $208=$3;
      var $209=((($208)+(48))|0);
      var $210=$ssize;
      var $211=((($209)-($210))|0);
      var $212=HEAP32[((((64)|0))>>2)];
      var $213=((($212)-(1))|0);
      var $214=((($211)+($213))|0);
      var $215=HEAP32[((((64)|0))>>2)];
      var $216=((($215)-(1))|0);
      var $217=$216 ^ -1;
      var $218=$214 & $217;
      $esize=$218;
      var $219=$esize;
      var $220=(($219)>>>(0)) < 2147483647;
      if ($220) { label = 42; break; } else { label = 46; break; }
    case 42: 
      var $222=$esize;
      var $223=_sbrk($222);
      $end=$223;
      var $224=$end;
      var $225=(($224)|(0))!=-1;
      if ($225) { label = 43; break; } else { label = 44; break; }
    case 43: 
      var $227=$esize;
      var $228=$ssize;
      var $229=((($228)+($227))|0);
      $ssize=$229;
      label = 45; break;
    case 44: 
      var $231=$ssize;
      var $232=(((-$231))|0);
      var $233=_sbrk($232);
      $br=-1;
      label = 45; break;
    case 45: 
      label = 46; break;
    case 46: 
      label = 47; break;
    case 47: 
      label = 48; break;
    case 48: 
      var $238=$br;
      var $239=(($238)|(0))!=-1;
      if ($239) { label = 49; break; } else { label = 50; break; }
    case 49: 
      var $241=$br;
      $tbase=$241;
      var $242=$ssize;
      $tsize=$242;
      label = 51; break;
    case 50: 
      var $244=$2;
      var $245=(($244+444)|0);
      var $246=HEAP32[(($245)>>2)];
      var $247=$246 | 4;
      HEAP32[(($245)>>2)]=$247;
      label = 51; break;
    case 51: 
      label = 52; break;
    case 52: 
      label = 53; break;
    case 53: 
      var $251=$tbase;
      var $252=(($251)|(0))==-1;
      if ($252) { label = 54; break; } else { label = 63; break; }
    case 54: 
      var $254=$asize;
      var $255=(($254)>>>(0)) < 2147483647;
      if ($255) { label = 55; break; } else { label = 62; break; }
    case 55: 
      $br2=-1;
      $end3=-1;
      var $257=$asize;
      var $258=_sbrk($257);
      $br2=$258;
      var $259=_sbrk(0);
      $end3=$259;
      var $260=$br2;
      var $261=(($260)|(0))!=-1;
      if ($261) { label = 56; break; } else { label = 61; break; }
    case 56: 
      var $263=$end3;
      var $264=(($263)|(0))!=-1;
      if ($264) { label = 57; break; } else { label = 61; break; }
    case 57: 
      var $266=$br2;
      var $267=$end3;
      var $268=(($266)>>>(0)) < (($267)>>>(0));
      if ($268) { label = 58; break; } else { label = 61; break; }
    case 58: 
      var $270=$end3;
      var $271=$br2;
      var $272=$270;
      var $273=$271;
      var $274=((($272)-($273))|0);
      $ssize4=$274;
      var $275=$ssize4;
      var $276=$3;
      var $277=((($276)+(40))|0);
      var $278=(($275)>>>(0)) > (($277)>>>(0));
      if ($278) { label = 59; break; } else { label = 60; break; }
    case 59: 
      var $280=$br2;
      $tbase=$280;
      var $281=$ssize4;
      $tsize=$281;
      label = 60; break;
    case 60: 
      label = 61; break;
    case 61: 
      label = 62; break;
    case 62: 
      label = 63; break;
    case 63: 
      var $286=$tbase;
      var $287=(($286)|(0))!=-1;
      if ($287) { label = 64; break; } else { label = 102; break; }
    case 64: 
      var $289=$tsize;
      var $290=$2;
      var $291=(($290+432)|0);
      var $292=HEAP32[(($291)>>2)];
      var $293=((($292)+($289))|0);
      HEAP32[(($291)>>2)]=$293;
      var $294=$2;
      var $295=(($294+436)|0);
      var $296=HEAP32[(($295)>>2)];
      var $297=(($293)>>>(0)) > (($296)>>>(0));
      if ($297) { label = 65; break; } else { label = 66; break; }
    case 65: 
      var $299=$2;
      var $300=(($299+432)|0);
      var $301=HEAP32[(($300)>>2)];
      var $302=$2;
      var $303=(($302+436)|0);
      HEAP32[(($303)>>2)]=$301;
      label = 66; break;
    case 66: 
      var $305=$2;
      var $306=(($305+24)|0);
      var $307=HEAP32[(($306)>>2)];
      var $308=(($307)|(0))!=0;
      if ($308) { label = 74; break; } else { label = 67; break; }
    case 67: 
      var $310=$2;
      var $311=(($310+16)|0);
      var $312=HEAP32[(($311)>>2)];
      var $313=(($312)|(0))==0;
      if ($313) { label = 69; break; } else { label = 68; break; }
    case 68: 
      var $315=$tbase;
      var $316=$2;
      var $317=(($316+16)|0);
      var $318=HEAP32[(($317)>>2)];
      var $319=(($315)>>>(0)) < (($318)>>>(0));
      if ($319) { label = 69; break; } else { label = 70; break; }
    case 69: 
      var $321=$tbase;
      var $322=$2;
      var $323=(($322+16)|0);
      HEAP32[(($323)>>2)]=$321;
      label = 70; break;
    case 70: 
      var $325=$tbase;
      var $326=$2;
      var $327=(($326+448)|0);
      var $328=(($327)|0);
      HEAP32[(($328)>>2)]=$325;
      var $329=$tsize;
      var $330=$2;
      var $331=(($330+448)|0);
      var $332=(($331+4)|0);
      HEAP32[(($332)>>2)]=$329;
      var $333=$mmap_flag;
      var $334=$2;
      var $335=(($334+448)|0);
      var $336=(($335+12)|0);
      HEAP32[(($336)>>2)]=$333;
      var $337=HEAP32[((((56)|0))>>2)];
      var $338=$2;
      var $339=(($338+36)|0);
      HEAP32[(($339)>>2)]=$337;
      var $340=$2;
      var $341=(($340+32)|0);
      HEAP32[(($341)>>2)]=-1;
      var $342=$2;
      _init_bins($342);
      var $343=$2;
      var $344=(($343)|(0))==408;
      if ($344) { label = 71; break; } else { label = 72; break; }
    case 71: 
      var $346=$2;
      var $347=$tbase;
      var $348=$347;
      var $349=$tsize;
      var $350=((($349)-(40))|0);
      _init_top($346, $348, $350);
      label = 73; break;
    case 72: 
      var $352=$2;
      var $353=$352;
      var $354=((($353)-(8))|0);
      var $355=$354;
      var $356=$355;
      var $357=$2;
      var $358=$357;
      var $359=((($358)-(8))|0);
      var $360=$359;
      var $361=(($360+4)|0);
      var $362=HEAP32[(($361)>>2)];
      var $363=$362 & -8;
      var $364=(($356+$363)|0);
      var $365=$364;
      $mn=$365;
      var $366=$2;
      var $367=$mn;
      var $368=$tbase;
      var $369=$tsize;
      var $370=(($368+$369)|0);
      var $371=$mn;
      var $372=$371;
      var $373=$370;
      var $374=$372;
      var $375=((($373)-($374))|0);
      var $376=((($375)-(40))|0);
      _init_top($366, $367, $376);
      label = 73; break;
    case 73: 
      label = 99; break;
    case 74: 
      var $379=$2;
      var $380=(($379+448)|0);
      $sp=$380;
      label = 75; break;
    case 75: 
      var $382=$sp;
      var $383=(($382)|(0))!=0;
      if ($383) { label = 76; break; } else { var $395 = 0;label = 77; break; }
    case 76: 
      var $385=$tbase;
      var $386=$sp;
      var $387=(($386)|0);
      var $388=HEAP32[(($387)>>2)];
      var $389=$sp;
      var $390=(($389+4)|0);
      var $391=HEAP32[(($390)>>2)];
      var $392=(($388+$391)|0);
      var $393=(($385)|(0))!=(($392)|(0));
      var $395 = $393;label = 77; break;
    case 77: 
      var $395;
      if ($395) { label = 78; break; } else { label = 79; break; }
    case 78: 
      var $397=$sp;
      var $398=(($397+8)|0);
      var $399=HEAP32[(($398)>>2)];
      $sp=$399;
      label = 75; break;
    case 79: 
      var $401=$sp;
      var $402=(($401)|(0))!=0;
      if ($402) { label = 80; break; } else { label = 85; break; }
    case 80: 
      var $404=$sp;
      var $405=(($404+12)|0);
      var $406=HEAP32[(($405)>>2)];
      var $407=$406 & 8;
      var $408=(($407)|(0))!=0;
      if ($408) { label = 85; break; } else { label = 81; break; }
    case 81: 
      var $410=$sp;
      var $411=(($410+12)|0);
      var $412=HEAP32[(($411)>>2)];
      var $413=$412 & 0;
      var $414=$mmap_flag;
      var $415=(($413)|(0))==(($414)|(0));
      if ($415) { label = 82; break; } else { label = 85; break; }
    case 82: 
      var $417=$2;
      var $418=(($417+24)|0);
      var $419=HEAP32[(($418)>>2)];
      var $420=$419;
      var $421=$sp;
      var $422=(($421)|0);
      var $423=HEAP32[(($422)>>2)];
      var $424=(($420)>>>(0)) >= (($423)>>>(0));
      if ($424) { label = 83; break; } else { label = 85; break; }
    case 83: 
      var $426=$2;
      var $427=(($426+24)|0);
      var $428=HEAP32[(($427)>>2)];
      var $429=$428;
      var $430=$sp;
      var $431=(($430)|0);
      var $432=HEAP32[(($431)>>2)];
      var $433=$sp;
      var $434=(($433+4)|0);
      var $435=HEAP32[(($434)>>2)];
      var $436=(($432+$435)|0);
      var $437=(($429)>>>(0)) < (($436)>>>(0));
      if ($437) { label = 84; break; } else { label = 85; break; }
    case 84: 
      var $439=$tsize;
      var $440=$sp;
      var $441=(($440+4)|0);
      var $442=HEAP32[(($441)>>2)];
      var $443=((($442)+($439))|0);
      HEAP32[(($441)>>2)]=$443;
      var $444=$2;
      var $445=$2;
      var $446=(($445+24)|0);
      var $447=HEAP32[(($446)>>2)];
      var $448=$2;
      var $449=(($448+12)|0);
      var $450=HEAP32[(($449)>>2)];
      var $451=$tsize;
      var $452=((($450)+($451))|0);
      _init_top($444, $447, $452);
      label = 98; break;
    case 85: 
      var $454=$tbase;
      var $455=$2;
      var $456=(($455+16)|0);
      var $457=HEAP32[(($456)>>2)];
      var $458=(($454)>>>(0)) < (($457)>>>(0));
      if ($458) { label = 86; break; } else { label = 87; break; }
    case 86: 
      var $460=$tbase;
      var $461=$2;
      var $462=(($461+16)|0);
      HEAP32[(($462)>>2)]=$460;
      label = 87; break;
    case 87: 
      var $464=$2;
      var $465=(($464+448)|0);
      $sp=$465;
      label = 88; break;
    case 88: 
      var $467=$sp;
      var $468=(($467)|(0))!=0;
      if ($468) { label = 89; break; } else { var $478 = 0;label = 90; break; }
    case 89: 
      var $470=$sp;
      var $471=(($470)|0);
      var $472=HEAP32[(($471)>>2)];
      var $473=$tbase;
      var $474=$tsize;
      var $475=(($473+$474)|0);
      var $476=(($472)|(0))!=(($475)|(0));
      var $478 = $476;label = 90; break;
    case 90: 
      var $478;
      if ($478) { label = 91; break; } else { label = 92; break; }
    case 91: 
      var $480=$sp;
      var $481=(($480+8)|0);
      var $482=HEAP32[(($481)>>2)];
      $sp=$482;
      label = 88; break;
    case 92: 
      var $484=$sp;
      var $485=(($484)|(0))!=0;
      if ($485) { label = 93; break; } else { label = 96; break; }
    case 93: 
      var $487=$sp;
      var $488=(($487+12)|0);
      var $489=HEAP32[(($488)>>2)];
      var $490=$489 & 8;
      var $491=(($490)|(0))!=0;
      if ($491) { label = 96; break; } else { label = 94; break; }
    case 94: 
      var $493=$sp;
      var $494=(($493+12)|0);
      var $495=HEAP32[(($494)>>2)];
      var $496=$495 & 0;
      var $497=$mmap_flag;
      var $498=(($496)|(0))==(($497)|(0));
      if ($498) { label = 95; break; } else { label = 96; break; }
    case 95: 
      var $500=$sp;
      var $501=(($500)|0);
      var $502=HEAP32[(($501)>>2)];
      $oldbase=$502;
      var $503=$tbase;
      var $504=$sp;
      var $505=(($504)|0);
      HEAP32[(($505)>>2)]=$503;
      var $506=$tsize;
      var $507=$sp;
      var $508=(($507+4)|0);
      var $509=HEAP32[(($508)>>2)];
      var $510=((($509)+($506))|0);
      HEAP32[(($508)>>2)]=$510;
      var $511=$2;
      var $512=$tbase;
      var $513=$oldbase;
      var $514=$3;
      var $515=_prepend_alloc($511, $512, $513, $514);
      $1=$515;
      label = 103; break;
    case 96: 
      var $517=$2;
      var $518=$tbase;
      var $519=$tsize;
      var $520=$mmap_flag;
      _add_segment($517, $518, $519, $520);
      label = 97; break;
    case 97: 
      label = 98; break;
    case 98: 
      label = 99; break;
    case 99: 
      var $524=$3;
      var $525=$2;
      var $526=(($525+12)|0);
      var $527=HEAP32[(($526)>>2)];
      var $528=(($524)>>>(0)) < (($527)>>>(0));
      if ($528) { label = 100; break; } else { label = 101; break; }
    case 100: 
      var $530=$3;
      var $531=$2;
      var $532=(($531+12)|0);
      var $533=HEAP32[(($532)>>2)];
      var $534=((($533)-($530))|0);
      HEAP32[(($532)>>2)]=$534;
      $rsize=$534;
      var $535=$2;
      var $536=(($535+24)|0);
      var $537=HEAP32[(($536)>>2)];
      $p=$537;
      var $538=$p;
      var $539=$538;
      var $540=$3;
      var $541=(($539+$540)|0);
      var $542=$541;
      var $543=$2;
      var $544=(($543+24)|0);
      HEAP32[(($544)>>2)]=$542;
      $r=$542;
      var $545=$rsize;
      var $546=$545 | 1;
      var $547=$r;
      var $548=(($547+4)|0);
      HEAP32[(($548)>>2)]=$546;
      var $549=$3;
      var $550=$549 | 1;
      var $551=$550 | 2;
      var $552=$p;
      var $553=(($552+4)|0);
      HEAP32[(($553)>>2)]=$551;
      var $554=$p;
      var $555=$554;
      var $556=(($555+8)|0);
      $1=$556;
      label = 103; break;
    case 101: 
      label = 102; break;
    case 102: 
      var $559=___errno_location();
      HEAP32[(($559)>>2)]=12;
      $1=0;
      label = 103; break;
    case 103: 
      var $561=$1;
      return $561;
    default: assert(0, "bad label: " + label);
  }
}
function _free($mem) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $p;
      var $psize;
      var $next;
      var $prevsize;
      var $prev;
      var $F;
      var $B;
      var $I;
      var $TP;
      var $XP;
      var $R;
      var $F1;
      var $RP;
      var $CP;
      var $H;
      var $C0;
      var $C1;
      var $tsize;
      var $dsize;
      var $nsize;
      var $F2;
      var $B3;
      var $I4;
      var $TP5;
      var $XP6;
      var $R7;
      var $F8;
      var $RP9;
      var $CP10;
      var $H11;
      var $C012;
      var $C113;
      var $I14;
      var $B15;
      var $F16;
      var $tp;
      var $H17;
      var $I18;
      var $X;
      var $Y;
      var $N;
      var $K;
      var $T;
      var $K19;
      var $C;
      var $F20;
      $1=$mem;
      var $2=$1;
      var $3=(($2)|(0))!=0;
      if ($3) { label = 2; break; } else { label = 214; break; }
    case 2: 
      var $5=$1;
      var $6=((($5)-(8))|0);
      var $7=$6;
      $p=$7;
      var $8=$p;
      var $9=$8;
      var $10=HEAP32[((((424)|0))>>2)];
      var $11=(($9)>>>(0)) >= (($10)>>>(0));
      if ($11) { label = 3; break; } else { var $19 = 0;label = 4; break; }
    case 3: 
      var $13=$p;
      var $14=(($13+4)|0);
      var $15=HEAP32[(($14)>>2)];
      var $16=$15 & 3;
      var $17=(($16)|(0))!=1;
      var $19 = $17;label = 4; break;
    case 4: 
      var $19;
      var $20=(($19)&(1));
      var $21=($20);
      var $22=(($21)|(0))!=0;
      if ($22) { label = 5; break; } else { label = 211; break; }
    case 5: 
      var $24=$p;
      var $25=(($24+4)|0);
      var $26=HEAP32[(($25)>>2)];
      var $27=$26 & -8;
      $psize=$27;
      var $28=$p;
      var $29=$28;
      var $30=$psize;
      var $31=(($29+$30)|0);
      var $32=$31;
      $next=$32;
      var $33=$p;
      var $34=(($33+4)|0);
      var $35=HEAP32[(($34)>>2)];
      var $36=$35 & 1;
      var $37=(($36)|(0))!=0;
      if ($37) { label = 86; break; } else { label = 6; break; }
    case 6: 
      var $39=$p;
      var $40=(($39)|0);
      var $41=HEAP32[(($40)>>2)];
      $prevsize=$41;
      var $42=$p;
      var $43=(($42+4)|0);
      var $44=HEAP32[(($43)>>2)];
      var $45=$44 & 3;
      var $46=(($45)|(0))==0;
      if ($46) { label = 7; break; } else { label = 8; break; }
    case 7: 
      var $48=$prevsize;
      var $49=((($48)+(16))|0);
      var $50=$psize;
      var $51=((($50)+($49))|0);
      $psize=$51;
      label = 213; break;
    case 8: 
      var $53=$p;
      var $54=$53;
      var $55=$prevsize;
      var $56=(((-$55))|0);
      var $57=(($54+$56)|0);
      var $58=$57;
      $prev=$58;
      var $59=$prevsize;
      var $60=$psize;
      var $61=((($60)+($59))|0);
      $psize=$61;
      var $62=$prev;
      $p=$62;
      var $63=$prev;
      var $64=$63;
      var $65=HEAP32[((((424)|0))>>2)];
      var $66=(($64)>>>(0)) >= (($65)>>>(0));
      var $67=(($66)&(1));
      var $68=($67);
      var $69=(($68)|(0))!=0;
      if ($69) { label = 9; break; } else { label = 83; break; }
    case 9: 
      var $71=$p;
      var $72=HEAP32[((((428)|0))>>2)];
      var $73=(($71)|(0))!=(($72)|(0));
      if ($73) { label = 10; break; } else { label = 79; break; }
    case 10: 
      var $75=$prevsize;
      var $76=$75 >>> 3;
      var $77=(($76)>>>(0)) < 32;
      if ($77) { label = 11; break; } else { label = 29; break; }
    case 11: 
      var $79=$p;
      var $80=(($79+8)|0);
      var $81=HEAP32[(($80)>>2)];
      $F=$81;
      var $82=$p;
      var $83=(($82+12)|0);
      var $84=HEAP32[(($83)>>2)];
      $B=$84;
      var $85=$prevsize;
      var $86=$85 >>> 3;
      $I=$86;
      var $87=$F;
      var $88=$I;
      var $89=$88 << 1;
      var $90=((((448)|0)+($89<<2))|0);
      var $91=$90;
      var $92=$91;
      var $93=(($87)|(0))==(($92)|(0));
      if ($93) { var $108 = 1;label = 15; break; } else { label = 12; break; }
    case 12: 
      var $95=$F;
      var $96=$95;
      var $97=HEAP32[((((424)|0))>>2)];
      var $98=(($96)>>>(0)) >= (($97)>>>(0));
      if ($98) { label = 13; break; } else { var $106 = 0;label = 14; break; }
    case 13: 
      var $100=$F;
      var $101=(($100+12)|0);
      var $102=HEAP32[(($101)>>2)];
      var $103=$p;
      var $104=(($102)|(0))==(($103)|(0));
      var $106 = $104;label = 14; break;
    case 14: 
      var $106;
      var $108 = $106;label = 15; break;
    case 15: 
      var $108;
      var $109=(($108)&(1));
      var $110=($109);
      var $111=(($110)|(0))!=0;
      if ($111) { label = 16; break; } else { label = 27; break; }
    case 16: 
      var $113=$B;
      var $114=$F;
      var $115=(($113)|(0))==(($114)|(0));
      if ($115) { label = 17; break; } else { label = 18; break; }
    case 17: 
      var $117=$I;
      var $118=1 << $117;
      var $119=$118 ^ -1;
      var $120=HEAP32[((((408)|0))>>2)];
      var $121=$120 & $119;
      HEAP32[((((408)|0))>>2)]=$121;
      label = 26; break;
    case 18: 
      var $123=$B;
      var $124=$I;
      var $125=$124 << 1;
      var $126=((((448)|0)+($125<<2))|0);
      var $127=$126;
      var $128=$127;
      var $129=(($123)|(0))==(($128)|(0));
      if ($129) { var $144 = 1;label = 22; break; } else { label = 19; break; }
    case 19: 
      var $131=$B;
      var $132=$131;
      var $133=HEAP32[((((424)|0))>>2)];
      var $134=(($132)>>>(0)) >= (($133)>>>(0));
      if ($134) { label = 20; break; } else { var $142 = 0;label = 21; break; }
    case 20: 
      var $136=$B;
      var $137=(($136+8)|0);
      var $138=HEAP32[(($137)>>2)];
      var $139=$p;
      var $140=(($138)|(0))==(($139)|(0));
      var $142 = $140;label = 21; break;
    case 21: 
      var $142;
      var $144 = $142;label = 22; break;
    case 22: 
      var $144;
      var $145=(($144)&(1));
      var $146=($145);
      var $147=(($146)|(0))!=0;
      if ($147) { label = 23; break; } else { label = 24; break; }
    case 23: 
      var $149=$B;
      var $150=$F;
      var $151=(($150+12)|0);
      HEAP32[(($151)>>2)]=$149;
      var $152=$F;
      var $153=$B;
      var $154=(($153+8)|0);
      HEAP32[(($154)>>2)]=$152;
      label = 25; break;
    case 24: 
      _abort();
      throw "Reached an unreachable!";
    case 25: 
      label = 26; break;
    case 26: 
      label = 28; break;
    case 27: 
      _abort();
      throw "Reached an unreachable!";
    case 28: 
      label = 78; break;
    case 29: 
      var $161=$p;
      var $162=$161;
      $TP=$162;
      var $163=$TP;
      var $164=(($163+24)|0);
      var $165=HEAP32[(($164)>>2)];
      $XP=$165;
      var $166=$TP;
      var $167=(($166+12)|0);
      var $168=HEAP32[(($167)>>2)];
      var $169=$TP;
      var $170=(($168)|(0))!=(($169)|(0));
      if ($170) { label = 30; break; } else { label = 37; break; }
    case 30: 
      var $172=$TP;
      var $173=(($172+8)|0);
      var $174=HEAP32[(($173)>>2)];
      $F1=$174;
      var $175=$TP;
      var $176=(($175+12)|0);
      var $177=HEAP32[(($176)>>2)];
      $R=$177;
      var $178=$F1;
      var $179=$178;
      var $180=HEAP32[((((424)|0))>>2)];
      var $181=(($179)>>>(0)) >= (($180)>>>(0));
      if ($181) { label = 31; break; } else { var $195 = 0;label = 33; break; }
    case 31: 
      var $183=$F1;
      var $184=(($183+12)|0);
      var $185=HEAP32[(($184)>>2)];
      var $186=$TP;
      var $187=(($185)|(0))==(($186)|(0));
      if ($187) { label = 32; break; } else { var $195 = 0;label = 33; break; }
    case 32: 
      var $189=$R;
      var $190=(($189+8)|0);
      var $191=HEAP32[(($190)>>2)];
      var $192=$TP;
      var $193=(($191)|(0))==(($192)|(0));
      var $195 = $193;label = 33; break;
    case 33: 
      var $195;
      var $196=(($195)&(1));
      var $197=($196);
      var $198=(($197)|(0))!=0;
      if ($198) { label = 34; break; } else { label = 35; break; }
    case 34: 
      var $200=$R;
      var $201=$F1;
      var $202=(($201+12)|0);
      HEAP32[(($202)>>2)]=$200;
      var $203=$F1;
      var $204=$R;
      var $205=(($204+8)|0);
      HEAP32[(($205)>>2)]=$203;
      label = 36; break;
    case 35: 
      _abort();
      throw "Reached an unreachable!";
    case 36: 
      label = 49; break;
    case 37: 
      var $209=$TP;
      var $210=(($209+16)|0);
      var $211=(($210+4)|0);
      $RP=$211;
      var $212=HEAP32[(($211)>>2)];
      $R=$212;
      var $213=(($212)|(0))!=0;
      if ($213) { label = 39; break; } else { label = 38; break; }
    case 38: 
      var $215=$TP;
      var $216=(($215+16)|0);
      var $217=(($216)|0);
      $RP=$217;
      var $218=HEAP32[(($217)>>2)];
      $R=$218;
      var $219=(($218)|(0))!=0;
      if ($219) { label = 39; break; } else { label = 48; break; }
    case 39: 
      label = 40; break;
    case 40: 
      var $222=$R;
      var $223=(($222+16)|0);
      var $224=(($223+4)|0);
      $CP=$224;
      var $225=HEAP32[(($224)>>2)];
      var $226=(($225)|(0))!=0;
      if ($226) { var $234 = 1;label = 42; break; } else { label = 41; break; }
    case 41: 
      var $228=$R;
      var $229=(($228+16)|0);
      var $230=(($229)|0);
      $CP=$230;
      var $231=HEAP32[(($230)>>2)];
      var $232=(($231)|(0))!=0;
      var $234 = $232;label = 42; break;
    case 42: 
      var $234;
      if ($234) { label = 43; break; } else { label = 44; break; }
    case 43: 
      var $236=$CP;
      $RP=$236;
      var $237=HEAP32[(($236)>>2)];
      $R=$237;
      label = 40; break;
    case 44: 
      var $239=$RP;
      var $240=$239;
      var $241=HEAP32[((((424)|0))>>2)];
      var $242=(($240)>>>(0)) >= (($241)>>>(0));
      var $243=(($242)&(1));
      var $244=($243);
      var $245=(($244)|(0))!=0;
      if ($245) { label = 45; break; } else { label = 46; break; }
    case 45: 
      var $247=$RP;
      HEAP32[(($247)>>2)]=0;
      label = 47; break;
    case 46: 
      _abort();
      throw "Reached an unreachable!";
    case 47: 
      label = 48; break;
    case 48: 
      label = 49; break;
    case 49: 
      var $252=$XP;
      var $253=(($252)|(0))!=0;
      if ($253) { label = 50; break; } else { label = 77; break; }
    case 50: 
      var $255=$TP;
      var $256=(($255+28)|0);
      var $257=HEAP32[(($256)>>2)];
      var $258=((((712)|0)+($257<<2))|0);
      $H=$258;
      var $259=$TP;
      var $260=$H;
      var $261=HEAP32[(($260)>>2)];
      var $262=(($259)|(0))==(($261)|(0));
      if ($262) { label = 51; break; } else { label = 54; break; }
    case 51: 
      var $264=$R;
      var $265=$H;
      HEAP32[(($265)>>2)]=$264;
      var $266=(($264)|(0))==0;
      if ($266) { label = 52; break; } else { label = 53; break; }
    case 52: 
      var $268=$TP;
      var $269=(($268+28)|0);
      var $270=HEAP32[(($269)>>2)];
      var $271=1 << $270;
      var $272=$271 ^ -1;
      var $273=HEAP32[((((412)|0))>>2)];
      var $274=$273 & $272;
      HEAP32[((((412)|0))>>2)]=$274;
      label = 53; break;
    case 53: 
      label = 61; break;
    case 54: 
      var $277=$XP;
      var $278=$277;
      var $279=HEAP32[((((424)|0))>>2)];
      var $280=(($278)>>>(0)) >= (($279)>>>(0));
      var $281=(($280)&(1));
      var $282=($281);
      var $283=(($282)|(0))!=0;
      if ($283) { label = 55; break; } else { label = 59; break; }
    case 55: 
      var $285=$XP;
      var $286=(($285+16)|0);
      var $287=(($286)|0);
      var $288=HEAP32[(($287)>>2)];
      var $289=$TP;
      var $290=(($288)|(0))==(($289)|(0));
      if ($290) { label = 56; break; } else { label = 57; break; }
    case 56: 
      var $292=$R;
      var $293=$XP;
      var $294=(($293+16)|0);
      var $295=(($294)|0);
      HEAP32[(($295)>>2)]=$292;
      label = 58; break;
    case 57: 
      var $297=$R;
      var $298=$XP;
      var $299=(($298+16)|0);
      var $300=(($299+4)|0);
      HEAP32[(($300)>>2)]=$297;
      label = 58; break;
    case 58: 
      label = 60; break;
    case 59: 
      _abort();
      throw "Reached an unreachable!";
    case 60: 
      label = 61; break;
    case 61: 
      var $305=$R;
      var $306=(($305)|(0))!=0;
      if ($306) { label = 62; break; } else { label = 76; break; }
    case 62: 
      var $308=$R;
      var $309=$308;
      var $310=HEAP32[((((424)|0))>>2)];
      var $311=(($309)>>>(0)) >= (($310)>>>(0));
      var $312=(($311)&(1));
      var $313=($312);
      var $314=(($313)|(0))!=0;
      if ($314) { label = 63; break; } else { label = 74; break; }
    case 63: 
      var $316=$XP;
      var $317=$R;
      var $318=(($317+24)|0);
      HEAP32[(($318)>>2)]=$316;
      var $319=$TP;
      var $320=(($319+16)|0);
      var $321=(($320)|0);
      var $322=HEAP32[(($321)>>2)];
      $C0=$322;
      var $323=(($322)|(0))!=0;
      if ($323) { label = 64; break; } else { label = 68; break; }
    case 64: 
      var $325=$C0;
      var $326=$325;
      var $327=HEAP32[((((424)|0))>>2)];
      var $328=(($326)>>>(0)) >= (($327)>>>(0));
      var $329=(($328)&(1));
      var $330=($329);
      var $331=(($330)|(0))!=0;
      if ($331) { label = 65; break; } else { label = 66; break; }
    case 65: 
      var $333=$C0;
      var $334=$R;
      var $335=(($334+16)|0);
      var $336=(($335)|0);
      HEAP32[(($336)>>2)]=$333;
      var $337=$R;
      var $338=$C0;
      var $339=(($338+24)|0);
      HEAP32[(($339)>>2)]=$337;
      label = 67; break;
    case 66: 
      _abort();
      throw "Reached an unreachable!";
    case 67: 
      label = 68; break;
    case 68: 
      var $343=$TP;
      var $344=(($343+16)|0);
      var $345=(($344+4)|0);
      var $346=HEAP32[(($345)>>2)];
      $C1=$346;
      var $347=(($346)|(0))!=0;
      if ($347) { label = 69; break; } else { label = 73; break; }
    case 69: 
      var $349=$C1;
      var $350=$349;
      var $351=HEAP32[((((424)|0))>>2)];
      var $352=(($350)>>>(0)) >= (($351)>>>(0));
      var $353=(($352)&(1));
      var $354=($353);
      var $355=(($354)|(0))!=0;
      if ($355) { label = 70; break; } else { label = 71; break; }
    case 70: 
      var $357=$C1;
      var $358=$R;
      var $359=(($358+16)|0);
      var $360=(($359+4)|0);
      HEAP32[(($360)>>2)]=$357;
      var $361=$R;
      var $362=$C1;
      var $363=(($362+24)|0);
      HEAP32[(($363)>>2)]=$361;
      label = 72; break;
    case 71: 
      _abort();
      throw "Reached an unreachable!";
    case 72: 
      label = 73; break;
    case 73: 
      label = 75; break;
    case 74: 
      _abort();
      throw "Reached an unreachable!";
    case 75: 
      label = 76; break;
    case 76: 
      label = 77; break;
    case 77: 
      label = 78; break;
    case 78: 
      label = 82; break;
    case 79: 
      var $373=$next;
      var $374=(($373+4)|0);
      var $375=HEAP32[(($374)>>2)];
      var $376=$375 & 3;
      var $377=(($376)|(0))==3;
      if ($377) { label = 80; break; } else { label = 81; break; }
    case 80: 
      var $379=$psize;
      HEAP32[((((416)|0))>>2)]=$379;
      var $380=$next;
      var $381=(($380+4)|0);
      var $382=HEAP32[(($381)>>2)];
      var $383=$382 & -2;
      HEAP32[(($381)>>2)]=$383;
      var $384=$psize;
      var $385=$384 | 1;
      var $386=$p;
      var $387=(($386+4)|0);
      HEAP32[(($387)>>2)]=$385;
      var $388=$psize;
      var $389=$p;
      var $390=$389;
      var $391=$psize;
      var $392=(($390+$391)|0);
      var $393=$392;
      var $394=(($393)|0);
      HEAP32[(($394)>>2)]=$388;
      label = 213; break;
    case 81: 
      label = 82; break;
    case 82: 
      label = 84; break;
    case 83: 
      label = 212; break;
    case 84: 
      label = 85; break;
    case 85: 
      label = 86; break;
    case 86: 
      var $401=$p;
      var $402=$401;
      var $403=$next;
      var $404=$403;
      var $405=(($402)>>>(0)) < (($404)>>>(0));
      if ($405) { label = 87; break; } else { var $413 = 0;label = 88; break; }
    case 87: 
      var $407=$next;
      var $408=(($407+4)|0);
      var $409=HEAP32[(($408)>>2)];
      var $410=$409 & 1;
      var $411=(($410)|(0))!=0;
      var $413 = $411;label = 88; break;
    case 88: 
      var $413;
      var $414=(($413)&(1));
      var $415=($414);
      var $416=(($415)|(0))!=0;
      if ($416) { label = 89; break; } else { label = 210; break; }
    case 89: 
      var $418=$next;
      var $419=(($418+4)|0);
      var $420=HEAP32[(($419)>>2)];
      var $421=$420 & 2;
      var $422=(($421)|(0))!=0;
      if ($422) { label = 171; break; } else { label = 90; break; }
    case 90: 
      var $424=$next;
      var $425=HEAP32[((((432)|0))>>2)];
      var $426=(($424)|(0))==(($425)|(0));
      if ($426) { label = 91; break; } else { label = 96; break; }
    case 91: 
      var $428=$psize;
      var $429=HEAP32[((((420)|0))>>2)];
      var $430=((($429)+($428))|0);
      HEAP32[((((420)|0))>>2)]=$430;
      $tsize=$430;
      var $431=$p;
      HEAP32[((((432)|0))>>2)]=$431;
      var $432=$tsize;
      var $433=$432 | 1;
      var $434=$p;
      var $435=(($434+4)|0);
      HEAP32[(($435)>>2)]=$433;
      var $436=$p;
      var $437=HEAP32[((((428)|0))>>2)];
      var $438=(($436)|(0))==(($437)|(0));
      if ($438) { label = 92; break; } else { label = 93; break; }
    case 92: 
      HEAP32[((((428)|0))>>2)]=0;
      HEAP32[((((416)|0))>>2)]=0;
      label = 93; break;
    case 93: 
      var $441=$tsize;
      var $442=HEAP32[((((436)|0))>>2)];
      var $443=(($441)>>>(0)) > (($442)>>>(0));
      if ($443) { label = 94; break; } else { label = 95; break; }
    case 94: 
      var $445=_sys_trim(408, 0);
      label = 95; break;
    case 95: 
      label = 213; break;
    case 96: 
      var $448=$next;
      var $449=HEAP32[((((428)|0))>>2)];
      var $450=(($448)|(0))==(($449)|(0));
      if ($450) { label = 97; break; } else { label = 98; break; }
    case 97: 
      var $452=$psize;
      var $453=HEAP32[((((416)|0))>>2)];
      var $454=((($453)+($452))|0);
      HEAP32[((((416)|0))>>2)]=$454;
      $dsize=$454;
      var $455=$p;
      HEAP32[((((428)|0))>>2)]=$455;
      var $456=$dsize;
      var $457=$456 | 1;
      var $458=$p;
      var $459=(($458+4)|0);
      HEAP32[(($459)>>2)]=$457;
      var $460=$dsize;
      var $461=$p;
      var $462=$461;
      var $463=$dsize;
      var $464=(($462+$463)|0);
      var $465=$464;
      var $466=(($465)|0);
      HEAP32[(($466)>>2)]=$460;
      label = 213; break;
    case 98: 
      var $468=$next;
      var $469=(($468+4)|0);
      var $470=HEAP32[(($469)>>2)];
      var $471=$470 & -8;
      $nsize=$471;
      var $472=$nsize;
      var $473=$psize;
      var $474=((($473)+($472))|0);
      $psize=$474;
      var $475=$nsize;
      var $476=$475 >>> 3;
      var $477=(($476)>>>(0)) < 32;
      if ($477) { label = 99; break; } else { label = 117; break; }
    case 99: 
      var $479=$next;
      var $480=(($479+8)|0);
      var $481=HEAP32[(($480)>>2)];
      $F2=$481;
      var $482=$next;
      var $483=(($482+12)|0);
      var $484=HEAP32[(($483)>>2)];
      $B3=$484;
      var $485=$nsize;
      var $486=$485 >>> 3;
      $I4=$486;
      var $487=$F2;
      var $488=$I4;
      var $489=$488 << 1;
      var $490=((((448)|0)+($489<<2))|0);
      var $491=$490;
      var $492=$491;
      var $493=(($487)|(0))==(($492)|(0));
      if ($493) { var $508 = 1;label = 103; break; } else { label = 100; break; }
    case 100: 
      var $495=$F2;
      var $496=$495;
      var $497=HEAP32[((((424)|0))>>2)];
      var $498=(($496)>>>(0)) >= (($497)>>>(0));
      if ($498) { label = 101; break; } else { var $506 = 0;label = 102; break; }
    case 101: 
      var $500=$F2;
      var $501=(($500+12)|0);
      var $502=HEAP32[(($501)>>2)];
      var $503=$next;
      var $504=(($502)|(0))==(($503)|(0));
      var $506 = $504;label = 102; break;
    case 102: 
      var $506;
      var $508 = $506;label = 103; break;
    case 103: 
      var $508;
      var $509=(($508)&(1));
      var $510=($509);
      var $511=(($510)|(0))!=0;
      if ($511) { label = 104; break; } else { label = 115; break; }
    case 104: 
      var $513=$B3;
      var $514=$F2;
      var $515=(($513)|(0))==(($514)|(0));
      if ($515) { label = 105; break; } else { label = 106; break; }
    case 105: 
      var $517=$I4;
      var $518=1 << $517;
      var $519=$518 ^ -1;
      var $520=HEAP32[((((408)|0))>>2)];
      var $521=$520 & $519;
      HEAP32[((((408)|0))>>2)]=$521;
      label = 114; break;
    case 106: 
      var $523=$B3;
      var $524=$I4;
      var $525=$524 << 1;
      var $526=((((448)|0)+($525<<2))|0);
      var $527=$526;
      var $528=$527;
      var $529=(($523)|(0))==(($528)|(0));
      if ($529) { var $544 = 1;label = 110; break; } else { label = 107; break; }
    case 107: 
      var $531=$B3;
      var $532=$531;
      var $533=HEAP32[((((424)|0))>>2)];
      var $534=(($532)>>>(0)) >= (($533)>>>(0));
      if ($534) { label = 108; break; } else { var $542 = 0;label = 109; break; }
    case 108: 
      var $536=$B3;
      var $537=(($536+8)|0);
      var $538=HEAP32[(($537)>>2)];
      var $539=$next;
      var $540=(($538)|(0))==(($539)|(0));
      var $542 = $540;label = 109; break;
    case 109: 
      var $542;
      var $544 = $542;label = 110; break;
    case 110: 
      var $544;
      var $545=(($544)&(1));
      var $546=($545);
      var $547=(($546)|(0))!=0;
      if ($547) { label = 111; break; } else { label = 112; break; }
    case 111: 
      var $549=$B3;
      var $550=$F2;
      var $551=(($550+12)|0);
      HEAP32[(($551)>>2)]=$549;
      var $552=$F2;
      var $553=$B3;
      var $554=(($553+8)|0);
      HEAP32[(($554)>>2)]=$552;
      label = 113; break;
    case 112: 
      _abort();
      throw "Reached an unreachable!";
    case 113: 
      label = 114; break;
    case 114: 
      label = 116; break;
    case 115: 
      _abort();
      throw "Reached an unreachable!";
    case 116: 
      label = 166; break;
    case 117: 
      var $561=$next;
      var $562=$561;
      $TP5=$562;
      var $563=$TP5;
      var $564=(($563+24)|0);
      var $565=HEAP32[(($564)>>2)];
      $XP6=$565;
      var $566=$TP5;
      var $567=(($566+12)|0);
      var $568=HEAP32[(($567)>>2)];
      var $569=$TP5;
      var $570=(($568)|(0))!=(($569)|(0));
      if ($570) { label = 118; break; } else { label = 125; break; }
    case 118: 
      var $572=$TP5;
      var $573=(($572+8)|0);
      var $574=HEAP32[(($573)>>2)];
      $F8=$574;
      var $575=$TP5;
      var $576=(($575+12)|0);
      var $577=HEAP32[(($576)>>2)];
      $R7=$577;
      var $578=$F8;
      var $579=$578;
      var $580=HEAP32[((((424)|0))>>2)];
      var $581=(($579)>>>(0)) >= (($580)>>>(0));
      if ($581) { label = 119; break; } else { var $595 = 0;label = 121; break; }
    case 119: 
      var $583=$F8;
      var $584=(($583+12)|0);
      var $585=HEAP32[(($584)>>2)];
      var $586=$TP5;
      var $587=(($585)|(0))==(($586)|(0));
      if ($587) { label = 120; break; } else { var $595 = 0;label = 121; break; }
    case 120: 
      var $589=$R7;
      var $590=(($589+8)|0);
      var $591=HEAP32[(($590)>>2)];
      var $592=$TP5;
      var $593=(($591)|(0))==(($592)|(0));
      var $595 = $593;label = 121; break;
    case 121: 
      var $595;
      var $596=(($595)&(1));
      var $597=($596);
      var $598=(($597)|(0))!=0;
      if ($598) { label = 122; break; } else { label = 123; break; }
    case 122: 
      var $600=$R7;
      var $601=$F8;
      var $602=(($601+12)|0);
      HEAP32[(($602)>>2)]=$600;
      var $603=$F8;
      var $604=$R7;
      var $605=(($604+8)|0);
      HEAP32[(($605)>>2)]=$603;
      label = 124; break;
    case 123: 
      _abort();
      throw "Reached an unreachable!";
    case 124: 
      label = 137; break;
    case 125: 
      var $609=$TP5;
      var $610=(($609+16)|0);
      var $611=(($610+4)|0);
      $RP9=$611;
      var $612=HEAP32[(($611)>>2)];
      $R7=$612;
      var $613=(($612)|(0))!=0;
      if ($613) { label = 127; break; } else { label = 126; break; }
    case 126: 
      var $615=$TP5;
      var $616=(($615+16)|0);
      var $617=(($616)|0);
      $RP9=$617;
      var $618=HEAP32[(($617)>>2)];
      $R7=$618;
      var $619=(($618)|(0))!=0;
      if ($619) { label = 127; break; } else { label = 136; break; }
    case 127: 
      label = 128; break;
    case 128: 
      var $622=$R7;
      var $623=(($622+16)|0);
      var $624=(($623+4)|0);
      $CP10=$624;
      var $625=HEAP32[(($624)>>2)];
      var $626=(($625)|(0))!=0;
      if ($626) { var $634 = 1;label = 130; break; } else { label = 129; break; }
    case 129: 
      var $628=$R7;
      var $629=(($628+16)|0);
      var $630=(($629)|0);
      $CP10=$630;
      var $631=HEAP32[(($630)>>2)];
      var $632=(($631)|(0))!=0;
      var $634 = $632;label = 130; break;
    case 130: 
      var $634;
      if ($634) { label = 131; break; } else { label = 132; break; }
    case 131: 
      var $636=$CP10;
      $RP9=$636;
      var $637=HEAP32[(($636)>>2)];
      $R7=$637;
      label = 128; break;
    case 132: 
      var $639=$RP9;
      var $640=$639;
      var $641=HEAP32[((((424)|0))>>2)];
      var $642=(($640)>>>(0)) >= (($641)>>>(0));
      var $643=(($642)&(1));
      var $644=($643);
      var $645=(($644)|(0))!=0;
      if ($645) { label = 133; break; } else { label = 134; break; }
    case 133: 
      var $647=$RP9;
      HEAP32[(($647)>>2)]=0;
      label = 135; break;
    case 134: 
      _abort();
      throw "Reached an unreachable!";
    case 135: 
      label = 136; break;
    case 136: 
      label = 137; break;
    case 137: 
      var $652=$XP6;
      var $653=(($652)|(0))!=0;
      if ($653) { label = 138; break; } else { label = 165; break; }
    case 138: 
      var $655=$TP5;
      var $656=(($655+28)|0);
      var $657=HEAP32[(($656)>>2)];
      var $658=((((712)|0)+($657<<2))|0);
      $H11=$658;
      var $659=$TP5;
      var $660=$H11;
      var $661=HEAP32[(($660)>>2)];
      var $662=(($659)|(0))==(($661)|(0));
      if ($662) { label = 139; break; } else { label = 142; break; }
    case 139: 
      var $664=$R7;
      var $665=$H11;
      HEAP32[(($665)>>2)]=$664;
      var $666=(($664)|(0))==0;
      if ($666) { label = 140; break; } else { label = 141; break; }
    case 140: 
      var $668=$TP5;
      var $669=(($668+28)|0);
      var $670=HEAP32[(($669)>>2)];
      var $671=1 << $670;
      var $672=$671 ^ -1;
      var $673=HEAP32[((((412)|0))>>2)];
      var $674=$673 & $672;
      HEAP32[((((412)|0))>>2)]=$674;
      label = 141; break;
    case 141: 
      label = 149; break;
    case 142: 
      var $677=$XP6;
      var $678=$677;
      var $679=HEAP32[((((424)|0))>>2)];
      var $680=(($678)>>>(0)) >= (($679)>>>(0));
      var $681=(($680)&(1));
      var $682=($681);
      var $683=(($682)|(0))!=0;
      if ($683) { label = 143; break; } else { label = 147; break; }
    case 143: 
      var $685=$XP6;
      var $686=(($685+16)|0);
      var $687=(($686)|0);
      var $688=HEAP32[(($687)>>2)];
      var $689=$TP5;
      var $690=(($688)|(0))==(($689)|(0));
      if ($690) { label = 144; break; } else { label = 145; break; }
    case 144: 
      var $692=$R7;
      var $693=$XP6;
      var $694=(($693+16)|0);
      var $695=(($694)|0);
      HEAP32[(($695)>>2)]=$692;
      label = 146; break;
    case 145: 
      var $697=$R7;
      var $698=$XP6;
      var $699=(($698+16)|0);
      var $700=(($699+4)|0);
      HEAP32[(($700)>>2)]=$697;
      label = 146; break;
    case 146: 
      label = 148; break;
    case 147: 
      _abort();
      throw "Reached an unreachable!";
    case 148: 
      label = 149; break;
    case 149: 
      var $705=$R7;
      var $706=(($705)|(0))!=0;
      if ($706) { label = 150; break; } else { label = 164; break; }
    case 150: 
      var $708=$R7;
      var $709=$708;
      var $710=HEAP32[((((424)|0))>>2)];
      var $711=(($709)>>>(0)) >= (($710)>>>(0));
      var $712=(($711)&(1));
      var $713=($712);
      var $714=(($713)|(0))!=0;
      if ($714) { label = 151; break; } else { label = 162; break; }
    case 151: 
      var $716=$XP6;
      var $717=$R7;
      var $718=(($717+24)|0);
      HEAP32[(($718)>>2)]=$716;
      var $719=$TP5;
      var $720=(($719+16)|0);
      var $721=(($720)|0);
      var $722=HEAP32[(($721)>>2)];
      $C012=$722;
      var $723=(($722)|(0))!=0;
      if ($723) { label = 152; break; } else { label = 156; break; }
    case 152: 
      var $725=$C012;
      var $726=$725;
      var $727=HEAP32[((((424)|0))>>2)];
      var $728=(($726)>>>(0)) >= (($727)>>>(0));
      var $729=(($728)&(1));
      var $730=($729);
      var $731=(($730)|(0))!=0;
      if ($731) { label = 153; break; } else { label = 154; break; }
    case 153: 
      var $733=$C012;
      var $734=$R7;
      var $735=(($734+16)|0);
      var $736=(($735)|0);
      HEAP32[(($736)>>2)]=$733;
      var $737=$R7;
      var $738=$C012;
      var $739=(($738+24)|0);
      HEAP32[(($739)>>2)]=$737;
      label = 155; break;
    case 154: 
      _abort();
      throw "Reached an unreachable!";
    case 155: 
      label = 156; break;
    case 156: 
      var $743=$TP5;
      var $744=(($743+16)|0);
      var $745=(($744+4)|0);
      var $746=HEAP32[(($745)>>2)];
      $C113=$746;
      var $747=(($746)|(0))!=0;
      if ($747) { label = 157; break; } else { label = 161; break; }
    case 157: 
      var $749=$C113;
      var $750=$749;
      var $751=HEAP32[((((424)|0))>>2)];
      var $752=(($750)>>>(0)) >= (($751)>>>(0));
      var $753=(($752)&(1));
      var $754=($753);
      var $755=(($754)|(0))!=0;
      if ($755) { label = 158; break; } else { label = 159; break; }
    case 158: 
      var $757=$C113;
      var $758=$R7;
      var $759=(($758+16)|0);
      var $760=(($759+4)|0);
      HEAP32[(($760)>>2)]=$757;
      var $761=$R7;
      var $762=$C113;
      var $763=(($762+24)|0);
      HEAP32[(($763)>>2)]=$761;
      label = 160; break;
    case 159: 
      _abort();
      throw "Reached an unreachable!";
    case 160: 
      label = 161; break;
    case 161: 
      label = 163; break;
    case 162: 
      _abort();
      throw "Reached an unreachable!";
    case 163: 
      label = 164; break;
    case 164: 
      label = 165; break;
    case 165: 
      label = 166; break;
    case 166: 
      var $772=$psize;
      var $773=$772 | 1;
      var $774=$p;
      var $775=(($774+4)|0);
      HEAP32[(($775)>>2)]=$773;
      var $776=$psize;
      var $777=$p;
      var $778=$777;
      var $779=$psize;
      var $780=(($778+$779)|0);
      var $781=$780;
      var $782=(($781)|0);
      HEAP32[(($782)>>2)]=$776;
      var $783=$p;
      var $784=HEAP32[((((428)|0))>>2)];
      var $785=(($783)|(0))==(($784)|(0));
      if ($785) { label = 167; break; } else { label = 168; break; }
    case 167: 
      var $787=$psize;
      HEAP32[((((416)|0))>>2)]=$787;
      label = 213; break;
    case 168: 
      label = 169; break;
    case 169: 
      label = 170; break;
    case 170: 
      label = 172; break;
    case 171: 
      var $792=$next;
      var $793=(($792+4)|0);
      var $794=HEAP32[(($793)>>2)];
      var $795=$794 & -2;
      HEAP32[(($793)>>2)]=$795;
      var $796=$psize;
      var $797=$796 | 1;
      var $798=$p;
      var $799=(($798+4)|0);
      HEAP32[(($799)>>2)]=$797;
      var $800=$psize;
      var $801=$p;
      var $802=$801;
      var $803=$psize;
      var $804=(($802+$803)|0);
      var $805=$804;
      var $806=(($805)|0);
      HEAP32[(($806)>>2)]=$800;
      label = 172; break;
    case 172: 
      var $808=$psize;
      var $809=$808 >>> 3;
      var $810=(($809)>>>(0)) < 32;
      if ($810) { label = 173; break; } else { label = 180; break; }
    case 173: 
      var $812=$psize;
      var $813=$812 >>> 3;
      $I14=$813;
      var $814=$I14;
      var $815=$814 << 1;
      var $816=((((448)|0)+($815<<2))|0);
      var $817=$816;
      var $818=$817;
      $B15=$818;
      var $819=$B15;
      $F16=$819;
      var $820=HEAP32[((((408)|0))>>2)];
      var $821=$I14;
      var $822=1 << $821;
      var $823=$820 & $822;
      var $824=(($823)|(0))!=0;
      if ($824) { label = 175; break; } else { label = 174; break; }
    case 174: 
      var $826=$I14;
      var $827=1 << $826;
      var $828=HEAP32[((((408)|0))>>2)];
      var $829=$828 | $827;
      HEAP32[((((408)|0))>>2)]=$829;
      label = 179; break;
    case 175: 
      var $831=$B15;
      var $832=(($831+8)|0);
      var $833=HEAP32[(($832)>>2)];
      var $834=$833;
      var $835=HEAP32[((((424)|0))>>2)];
      var $836=(($834)>>>(0)) >= (($835)>>>(0));
      var $837=(($836)&(1));
      var $838=($837);
      var $839=(($838)|(0))!=0;
      if ($839) { label = 176; break; } else { label = 177; break; }
    case 176: 
      var $841=$B15;
      var $842=(($841+8)|0);
      var $843=HEAP32[(($842)>>2)];
      $F16=$843;
      label = 178; break;
    case 177: 
      _abort();
      throw "Reached an unreachable!";
    case 178: 
      label = 179; break;
    case 179: 
      var $847=$p;
      var $848=$B15;
      var $849=(($848+8)|0);
      HEAP32[(($849)>>2)]=$847;
      var $850=$p;
      var $851=$F16;
      var $852=(($851+12)|0);
      HEAP32[(($852)>>2)]=$850;
      var $853=$F16;
      var $854=$p;
      var $855=(($854+8)|0);
      HEAP32[(($855)>>2)]=$853;
      var $856=$B15;
      var $857=$p;
      var $858=(($857+12)|0);
      HEAP32[(($858)>>2)]=$856;
      label = 209; break;
    case 180: 
      var $860=$p;
      var $861=$860;
      $tp=$861;
      var $862=$psize;
      var $863=$862 >>> 8;
      $X=$863;
      var $864=$X;
      var $865=(($864)|(0))==0;
      if ($865) { label = 181; break; } else { label = 182; break; }
    case 181: 
      $I18=0;
      label = 186; break;
    case 182: 
      var $868=$X;
      var $869=(($868)>>>(0)) > 65535;
      if ($869) { label = 183; break; } else { label = 184; break; }
    case 183: 
      $I18=31;
      label = 185; break;
    case 184: 
      var $872=$X;
      $Y=$872;
      var $873=$Y;
      var $874=((($873)-(256))|0);
      var $875=$874 >>> 16;
      var $876=$875 & 8;
      $N=$876;
      var $877=$N;
      var $878=$Y;
      var $879=$878 << $877;
      $Y=$879;
      var $880=((($879)-(4096))|0);
      var $881=$880 >>> 16;
      var $882=$881 & 4;
      $K=$882;
      var $883=$K;
      var $884=$N;
      var $885=((($884)+($883))|0);
      $N=$885;
      var $886=$K;
      var $887=$Y;
      var $888=$887 << $886;
      $Y=$888;
      var $889=((($888)-(16384))|0);
      var $890=$889 >>> 16;
      var $891=$890 & 2;
      $K=$891;
      var $892=$N;
      var $893=((($892)+($891))|0);
      $N=$893;
      var $894=$N;
      var $895=(((14)-($894))|0);
      var $896=$K;
      var $897=$Y;
      var $898=$897 << $896;
      $Y=$898;
      var $899=$898 >>> 15;
      var $900=((($895)+($899))|0);
      $K=$900;
      var $901=$K;
      var $902=$901 << 1;
      var $903=$psize;
      var $904=$K;
      var $905=((($904)+(7))|0);
      var $906=$903 >>> (($905)>>>(0));
      var $907=$906 & 1;
      var $908=((($902)+($907))|0);
      $I18=$908;
      label = 185; break;
    case 185: 
      label = 186; break;
    case 186: 
      var $911=$I18;
      var $912=((((712)|0)+($911<<2))|0);
      $H17=$912;
      var $913=$I18;
      var $914=$tp;
      var $915=(($914+28)|0);
      HEAP32[(($915)>>2)]=$913;
      var $916=$tp;
      var $917=(($916+16)|0);
      var $918=(($917+4)|0);
      HEAP32[(($918)>>2)]=0;
      var $919=$tp;
      var $920=(($919+16)|0);
      var $921=(($920)|0);
      HEAP32[(($921)>>2)]=0;
      var $922=HEAP32[((((412)|0))>>2)];
      var $923=$I18;
      var $924=1 << $923;
      var $925=$922 & $924;
      var $926=(($925)|(0))!=0;
      if ($926) { label = 188; break; } else { label = 187; break; }
    case 187: 
      var $928=$I18;
      var $929=1 << $928;
      var $930=HEAP32[((((412)|0))>>2)];
      var $931=$930 | $929;
      HEAP32[((((412)|0))>>2)]=$931;
      var $932=$tp;
      var $933=$H17;
      HEAP32[(($933)>>2)]=$932;
      var $934=$H17;
      var $935=$934;
      var $936=$tp;
      var $937=(($936+24)|0);
      HEAP32[(($937)>>2)]=$935;
      var $938=$tp;
      var $939=$tp;
      var $940=(($939+12)|0);
      HEAP32[(($940)>>2)]=$938;
      var $941=$tp;
      var $942=(($941+8)|0);
      HEAP32[(($942)>>2)]=$938;
      label = 206; break;
    case 188: 
      var $944=$H17;
      var $945=HEAP32[(($944)>>2)];
      $T=$945;
      var $946=$psize;
      var $947=$I18;
      var $948=(($947)|(0))==31;
      if ($948) { label = 189; break; } else { label = 190; break; }
    case 189: 
      var $957 = 0;label = 191; break;
    case 190: 
      var $951=$I18;
      var $952=$951 >>> 1;
      var $953=((($952)+(8))|0);
      var $954=((($953)-(2))|0);
      var $955=(((31)-($954))|0);
      var $957 = $955;label = 191; break;
    case 191: 
      var $957;
      var $958=$946 << $957;
      $K19=$958;
      label = 192; break;
    case 192: 
      var $960=$T;
      var $961=(($960+4)|0);
      var $962=HEAP32[(($961)>>2)];
      var $963=$962 & -8;
      var $964=$psize;
      var $965=(($963)|(0))!=(($964)|(0));
      if ($965) { label = 193; break; } else { label = 199; break; }
    case 193: 
      var $967=$K19;
      var $968=$967 >>> 31;
      var $969=$968 & 1;
      var $970=$T;
      var $971=(($970+16)|0);
      var $972=(($971+($969<<2))|0);
      $C=$972;
      var $973=$K19;
      var $974=$973 << 1;
      $K19=$974;
      var $975=$C;
      var $976=HEAP32[(($975)>>2)];
      var $977=(($976)|(0))!=0;
      if ($977) { label = 194; break; } else { label = 195; break; }
    case 194: 
      var $979=$C;
      var $980=HEAP32[(($979)>>2)];
      $T=$980;
      label = 198; break;
    case 195: 
      var $982=$C;
      var $983=$982;
      var $984=HEAP32[((((424)|0))>>2)];
      var $985=(($983)>>>(0)) >= (($984)>>>(0));
      var $986=(($985)&(1));
      var $987=($986);
      var $988=(($987)|(0))!=0;
      if ($988) { label = 196; break; } else { label = 197; break; }
    case 196: 
      var $990=$tp;
      var $991=$C;
      HEAP32[(($991)>>2)]=$990;
      var $992=$T;
      var $993=$tp;
      var $994=(($993+24)|0);
      HEAP32[(($994)>>2)]=$992;
      var $995=$tp;
      var $996=$tp;
      var $997=(($996+12)|0);
      HEAP32[(($997)>>2)]=$995;
      var $998=$tp;
      var $999=(($998+8)|0);
      HEAP32[(($999)>>2)]=$995;
      label = 205; break;
    case 197: 
      _abort();
      throw "Reached an unreachable!";
    case 198: 
      label = 204; break;
    case 199: 
      var $1003=$T;
      var $1004=(($1003+8)|0);
      var $1005=HEAP32[(($1004)>>2)];
      $F20=$1005;
      var $1006=$T;
      var $1007=$1006;
      var $1008=HEAP32[((((424)|0))>>2)];
      var $1009=(($1007)>>>(0)) >= (($1008)>>>(0));
      if ($1009) { label = 200; break; } else { var $1016 = 0;label = 201; break; }
    case 200: 
      var $1011=$F20;
      var $1012=$1011;
      var $1013=HEAP32[((((424)|0))>>2)];
      var $1014=(($1012)>>>(0)) >= (($1013)>>>(0));
      var $1016 = $1014;label = 201; break;
    case 201: 
      var $1016;
      var $1017=(($1016)&(1));
      var $1018=($1017);
      var $1019=(($1018)|(0))!=0;
      if ($1019) { label = 202; break; } else { label = 203; break; }
    case 202: 
      var $1021=$tp;
      var $1022=$F20;
      var $1023=(($1022+12)|0);
      HEAP32[(($1023)>>2)]=$1021;
      var $1024=$T;
      var $1025=(($1024+8)|0);
      HEAP32[(($1025)>>2)]=$1021;
      var $1026=$F20;
      var $1027=$tp;
      var $1028=(($1027+8)|0);
      HEAP32[(($1028)>>2)]=$1026;
      var $1029=$T;
      var $1030=$tp;
      var $1031=(($1030+12)|0);
      HEAP32[(($1031)>>2)]=$1029;
      var $1032=$tp;
      var $1033=(($1032+24)|0);
      HEAP32[(($1033)>>2)]=0;
      label = 205; break;
    case 203: 
      _abort();
      throw "Reached an unreachable!";
    case 204: 
      label = 192; break;
    case 205: 
      label = 206; break;
    case 206: 
      var $1038=HEAP32[((((440)|0))>>2)];
      var $1039=((($1038)-(1))|0);
      HEAP32[((((440)|0))>>2)]=$1039;
      var $1040=(($1039)|(0))==0;
      if ($1040) { label = 207; break; } else { label = 208; break; }
    case 207: 
      var $1042=_release_unused_segments(408);
      label = 208; break;
    case 208: 
      label = 209; break;
    case 209: 
      label = 213; break;
    case 210: 
      label = 211; break;
    case 211: 
      label = 212; break;
    case 212: 
      _abort();
      throw "Reached an unreachable!";
    case 213: 
      label = 214; break;
    case 214: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function _sys_trim($m, $pad) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $released;
      var $unit;
      var $extra;
      var $sp;
      var $old_br;
      var $rel_br;
      var $new_br;
      $1=$m;
      $2=$pad;
      $released=0;
      var $3=HEAP32[((((56)|0))>>2)];
      var $4=(($3)|(0))!=0;
      if ($4) { var $9 = 1;label = 3; break; } else { label = 2; break; }
    case 2: 
      var $6=_init_mparams();
      var $7=(($6)|(0))!=0;
      var $9 = $7;label = 3; break;
    case 3: 
      var $9;
      var $10=(($9)&(1));
      var $11=$2;
      var $12=(($11)>>>(0)) < 4294967232;
      if ($12) { label = 4; break; } else { label = 25; break; }
    case 4: 
      var $14=$1;
      var $15=(($14+24)|0);
      var $16=HEAP32[(($15)>>2)];
      var $17=(($16)|(0))!=0;
      if ($17) { label = 5; break; } else { label = 25; break; }
    case 5: 
      var $19=$2;
      var $20=((($19)+(40))|0);
      $2=$20;
      var $21=$1;
      var $22=(($21+12)|0);
      var $23=HEAP32[(($22)>>2)];
      var $24=$2;
      var $25=(($23)>>>(0)) > (($24)>>>(0));
      if ($25) { label = 6; break; } else { label = 21; break; }
    case 6: 
      var $27=HEAP32[((((64)|0))>>2)];
      $unit=$27;
      var $28=$1;
      var $29=(($28+12)|0);
      var $30=HEAP32[(($29)>>2)];
      var $31=$2;
      var $32=((($30)-($31))|0);
      var $33=$unit;
      var $34=((($33)-(1))|0);
      var $35=((($32)+($34))|0);
      var $36=$unit;
      var $37=Math.floor(((($35)>>>(0)))/((($36)>>>(0))));
      var $38=((($37)-(1))|0);
      var $39=$unit;
      var $40=Math.imul($38,$39);
      $extra=$40;
      var $41=$1;
      var $42=$1;
      var $43=(($42+24)|0);
      var $44=HEAP32[(($43)>>2)];
      var $45=$44;
      var $46=_segment_holding($41, $45);
      $sp=$46;
      var $47=$sp;
      var $48=(($47+12)|0);
      var $49=HEAP32[(($48)>>2)];
      var $50=$49 & 8;
      var $51=(($50)|(0))!=0;
      if ($51) { label = 18; break; } else { label = 7; break; }
    case 7: 
      var $53=$sp;
      var $54=(($53+12)|0);
      var $55=HEAP32[(($54)>>2)];
      var $56=$55 & 0;
      var $57=(($56)|(0))!=0;
      if ($57) { label = 8; break; } else { label = 9; break; }
    case 8: 
      label = 17; break;
    case 9: 
      var $60=$extra;
      var $61=(($60)>>>(0)) >= 2147483647;
      if ($61) { label = 10; break; } else { label = 11; break; }
    case 10: 
      var $63=$unit;
      var $64=(((-2147483648)-($63))|0);
      $extra=$64;
      label = 11; break;
    case 11: 
      var $66=_sbrk(0);
      $old_br=$66;
      var $67=$old_br;
      var $68=$sp;
      var $69=(($68)|0);
      var $70=HEAP32[(($69)>>2)];
      var $71=$sp;
      var $72=(($71+4)|0);
      var $73=HEAP32[(($72)>>2)];
      var $74=(($70+$73)|0);
      var $75=(($67)|(0))==(($74)|(0));
      if ($75) { label = 12; break; } else { label = 16; break; }
    case 12: 
      var $77=$extra;
      var $78=(((-$77))|0);
      var $79=_sbrk($78);
      $rel_br=$79;
      var $80=_sbrk(0);
      $new_br=$80;
      var $81=$rel_br;
      var $82=(($81)|(0))!=-1;
      if ($82) { label = 13; break; } else { label = 15; break; }
    case 13: 
      var $84=$new_br;
      var $85=$old_br;
      var $86=(($84)>>>(0)) < (($85)>>>(0));
      if ($86) { label = 14; break; } else { label = 15; break; }
    case 14: 
      var $88=$old_br;
      var $89=$new_br;
      var $90=$88;
      var $91=$89;
      var $92=((($90)-($91))|0);
      $released=$92;
      label = 15; break;
    case 15: 
      label = 16; break;
    case 16: 
      label = 17; break;
    case 17: 
      label = 18; break;
    case 18: 
      var $97=$released;
      var $98=(($97)|(0))!=0;
      if ($98) { label = 19; break; } else { label = 20; break; }
    case 19: 
      var $100=$released;
      var $101=$sp;
      var $102=(($101+4)|0);
      var $103=HEAP32[(($102)>>2)];
      var $104=((($103)-($100))|0);
      HEAP32[(($102)>>2)]=$104;
      var $105=$released;
      var $106=$1;
      var $107=(($106+432)|0);
      var $108=HEAP32[(($107)>>2)];
      var $109=((($108)-($105))|0);
      HEAP32[(($107)>>2)]=$109;
      var $110=$1;
      var $111=$1;
      var $112=(($111+24)|0);
      var $113=HEAP32[(($112)>>2)];
      var $114=$1;
      var $115=(($114+12)|0);
      var $116=HEAP32[(($115)>>2)];
      var $117=$released;
      var $118=((($116)-($117))|0);
      _init_top($110, $113, $118);
      label = 20; break;
    case 20: 
      label = 21; break;
    case 21: 
      var $121=$released;
      var $122=(($121)|(0))==0;
      if ($122) { label = 22; break; } else { label = 24; break; }
    case 22: 
      var $124=$1;
      var $125=(($124+12)|0);
      var $126=HEAP32[(($125)>>2)];
      var $127=$1;
      var $128=(($127+28)|0);
      var $129=HEAP32[(($128)>>2)];
      var $130=(($126)>>>(0)) > (($129)>>>(0));
      if ($130) { label = 23; break; } else { label = 24; break; }
    case 23: 
      var $132=$1;
      var $133=(($132+28)|0);
      HEAP32[(($133)>>2)]=-1;
      label = 24; break;
    case 24: 
      label = 25; break;
    case 25: 
      var $136=$released;
      var $137=(($136)|(0))!=0;
      var $138=$137 ? 1 : 0;
      return $138;
    default: assert(0, "bad label: " + label);
  }
}
function _segment_holding($m, $addr) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $sp;
      $2=$m;
      $3=$addr;
      var $4=$2;
      var $5=(($4+448)|0);
      $sp=$5;
      label = 2; break;
    case 2: 
      var $7=$3;
      var $8=$sp;
      var $9=(($8)|0);
      var $10=HEAP32[(($9)>>2)];
      var $11=(($7)>>>(0)) >= (($10)>>>(0));
      if ($11) { label = 3; break; } else { label = 5; break; }
    case 3: 
      var $13=$3;
      var $14=$sp;
      var $15=(($14)|0);
      var $16=HEAP32[(($15)>>2)];
      var $17=$sp;
      var $18=(($17+4)|0);
      var $19=HEAP32[(($18)>>2)];
      var $20=(($16+$19)|0);
      var $21=(($13)>>>(0)) < (($20)>>>(0));
      if ($21) { label = 4; break; } else { label = 5; break; }
    case 4: 
      var $23=$sp;
      $1=$23;
      label = 8; break;
    case 5: 
      var $25=$sp;
      var $26=(($25+8)|0);
      var $27=HEAP32[(($26)>>2)];
      $sp=$27;
      var $28=(($27)|(0))==0;
      if ($28) { label = 6; break; } else { label = 7; break; }
    case 6: 
      $1=0;
      label = 8; break;
    case 7: 
      label = 2; break;
    case 8: 
      var $32=$1;
      return $32;
    default: assert(0, "bad label: " + label);
  }
}
function _init_top($m, $p, $psize) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $offset;
      $1=$m;
      $2=$p;
      $3=$psize;
      var $4=$2;
      var $5=$4;
      var $6=(($5+8)|0);
      var $7=$6;
      var $8=$7 & 7;
      var $9=(($8)|(0))==0;
      if ($9) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $20 = 0;label = 4; break;
    case 3: 
      var $12=$2;
      var $13=$12;
      var $14=(($13+8)|0);
      var $15=$14;
      var $16=$15 & 7;
      var $17=(((8)-($16))|0);
      var $18=$17 & 7;
      var $20 = $18;label = 4; break;
    case 4: 
      var $20;
      $offset=$20;
      var $21=$2;
      var $22=$21;
      var $23=$offset;
      var $24=(($22+$23)|0);
      var $25=$24;
      $2=$25;
      var $26=$offset;
      var $27=$3;
      var $28=((($27)-($26))|0);
      $3=$28;
      var $29=$2;
      var $30=$1;
      var $31=(($30+24)|0);
      HEAP32[(($31)>>2)]=$29;
      var $32=$3;
      var $33=$1;
      var $34=(($33+12)|0);
      HEAP32[(($34)>>2)]=$32;
      var $35=$3;
      var $36=$35 | 1;
      var $37=$2;
      var $38=(($37+4)|0);
      HEAP32[(($38)>>2)]=$36;
      var $39=$2;
      var $40=$39;
      var $41=$3;
      var $42=(($40+$41)|0);
      var $43=$42;
      var $44=(($43+4)|0);
      HEAP32[(($44)>>2)]=40;
      var $45=HEAP32[((((72)|0))>>2)];
      var $46=$1;
      var $47=(($46+28)|0);
      HEAP32[(($47)>>2)]=$45;
      return;
    default: assert(0, "bad label: " + label);
  }
}
function _release_unused_segments($m) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $released;
      var $nsegs;
      var $pred;
      var $sp;
      var $base;
      var $size;
      var $next;
      var $p;
      var $psize;
      var $tp;
      var $XP;
      var $R;
      var $F;
      var $RP;
      var $CP;
      var $H;
      var $C0;
      var $C1;
      var $H1;
      var $I;
      var $X;
      var $Y;
      var $N;
      var $K;
      var $T;
      var $K2;
      var $C;
      var $F3;
      $1=$m;
      $released=0;
      $nsegs=0;
      var $2=$1;
      var $3=(($2+448)|0);
      $pred=$3;
      var $4=$pred;
      var $5=(($4+8)|0);
      var $6=HEAP32[(($5)>>2)];
      $sp=$6;
      label = 2; break;
    case 2: 
      var $8=$sp;
      var $9=(($8)|(0))!=0;
      if ($9) { label = 3; break; } else { label = 90; break; }
    case 3: 
      var $11=$sp;
      var $12=(($11)|0);
      var $13=HEAP32[(($12)>>2)];
      $base=$13;
      var $14=$sp;
      var $15=(($14+4)|0);
      var $16=HEAP32[(($15)>>2)];
      $size=$16;
      var $17=$sp;
      var $18=(($17+8)|0);
      var $19=HEAP32[(($18)>>2)];
      $next=$19;
      var $20=$nsegs;
      var $21=((($20)+(1))|0);
      $nsegs=$21;
      var $22=$sp;
      var $23=(($22+12)|0);
      var $24=HEAP32[(($23)>>2)];
      var $25=$24 & 0;
      var $26=(($25)|(0))!=0;
      if ($26) { label = 4; break; } else { label = 89; break; }
    case 4: 
      var $28=$sp;
      var $29=(($28+12)|0);
      var $30=HEAP32[(($29)>>2)];
      var $31=$30 & 8;
      var $32=(($31)|(0))!=0;
      if ($32) { label = 89; break; } else { label = 5; break; }
    case 5: 
      var $34=$base;
      var $35=$base;
      var $36=(($35+8)|0);
      var $37=$36;
      var $38=$37 & 7;
      var $39=(($38)|(0))==0;
      if ($39) { label = 6; break; } else { label = 7; break; }
    case 6: 
      var $49 = 0;label = 8; break;
    case 7: 
      var $42=$base;
      var $43=(($42+8)|0);
      var $44=$43;
      var $45=$44 & 7;
      var $46=(((8)-($45))|0);
      var $47=$46 & 7;
      var $49 = $47;label = 8; break;
    case 8: 
      var $49;
      var $50=(($34+$49)|0);
      var $51=$50;
      $p=$51;
      var $52=$p;
      var $53=(($52+4)|0);
      var $54=HEAP32[(($53)>>2)];
      var $55=$54 & -8;
      $psize=$55;
      var $56=$p;
      var $57=(($56+4)|0);
      var $58=HEAP32[(($57)>>2)];
      var $59=$58 & 3;
      var $60=(($59)|(0))!=1;
      if ($60) { label = 88; break; } else { label = 9; break; }
    case 9: 
      var $62=$p;
      var $63=$62;
      var $64=$psize;
      var $65=(($63+$64)|0);
      var $66=$base;
      var $67=$size;
      var $68=(($66+$67)|0);
      var $69=((($68)-(40))|0);
      var $70=(($65)>>>(0)) >= (($69)>>>(0));
      if ($70) { label = 10; break; } else { label = 88; break; }
    case 10: 
      var $72=$p;
      var $73=$72;
      $tp=$73;
      var $74=$p;
      var $75=$1;
      var $76=(($75+20)|0);
      var $77=HEAP32[(($76)>>2)];
      var $78=(($74)|(0))==(($77)|(0));
      if ($78) { label = 11; break; } else { label = 12; break; }
    case 11: 
      var $80=$1;
      var $81=(($80+20)|0);
      HEAP32[(($81)>>2)]=0;
      var $82=$1;
      var $83=(($82+8)|0);
      HEAP32[(($83)>>2)]=0;
      label = 61; break;
    case 12: 
      var $85=$tp;
      var $86=(($85+24)|0);
      var $87=HEAP32[(($86)>>2)];
      $XP=$87;
      var $88=$tp;
      var $89=(($88+12)|0);
      var $90=HEAP32[(($89)>>2)];
      var $91=$tp;
      var $92=(($90)|(0))!=(($91)|(0));
      if ($92) { label = 13; break; } else { label = 20; break; }
    case 13: 
      var $94=$tp;
      var $95=(($94+8)|0);
      var $96=HEAP32[(($95)>>2)];
      $F=$96;
      var $97=$tp;
      var $98=(($97+12)|0);
      var $99=HEAP32[(($98)>>2)];
      $R=$99;
      var $100=$F;
      var $101=$100;
      var $102=$1;
      var $103=(($102+16)|0);
      var $104=HEAP32[(($103)>>2)];
      var $105=(($101)>>>(0)) >= (($104)>>>(0));
      if ($105) { label = 14; break; } else { var $119 = 0;label = 16; break; }
    case 14: 
      var $107=$F;
      var $108=(($107+12)|0);
      var $109=HEAP32[(($108)>>2)];
      var $110=$tp;
      var $111=(($109)|(0))==(($110)|(0));
      if ($111) { label = 15; break; } else { var $119 = 0;label = 16; break; }
    case 15: 
      var $113=$R;
      var $114=(($113+8)|0);
      var $115=HEAP32[(($114)>>2)];
      var $116=$tp;
      var $117=(($115)|(0))==(($116)|(0));
      var $119 = $117;label = 16; break;
    case 16: 
      var $119;
      var $120=(($119)&(1));
      var $121=($120);
      var $122=(($121)|(0))!=0;
      if ($122) { label = 17; break; } else { label = 18; break; }
    case 17: 
      var $124=$R;
      var $125=$F;
      var $126=(($125+12)|0);
      HEAP32[(($126)>>2)]=$124;
      var $127=$F;
      var $128=$R;
      var $129=(($128+8)|0);
      HEAP32[(($129)>>2)]=$127;
      label = 19; break;
    case 18: 
      _abort();
      throw "Reached an unreachable!";
    case 19: 
      label = 32; break;
    case 20: 
      var $133=$tp;
      var $134=(($133+16)|0);
      var $135=(($134+4)|0);
      $RP=$135;
      var $136=HEAP32[(($135)>>2)];
      $R=$136;
      var $137=(($136)|(0))!=0;
      if ($137) { label = 22; break; } else { label = 21; break; }
    case 21: 
      var $139=$tp;
      var $140=(($139+16)|0);
      var $141=(($140)|0);
      $RP=$141;
      var $142=HEAP32[(($141)>>2)];
      $R=$142;
      var $143=(($142)|(0))!=0;
      if ($143) { label = 22; break; } else { label = 31; break; }
    case 22: 
      label = 23; break;
    case 23: 
      var $146=$R;
      var $147=(($146+16)|0);
      var $148=(($147+4)|0);
      $CP=$148;
      var $149=HEAP32[(($148)>>2)];
      var $150=(($149)|(0))!=0;
      if ($150) { var $158 = 1;label = 25; break; } else { label = 24; break; }
    case 24: 
      var $152=$R;
      var $153=(($152+16)|0);
      var $154=(($153)|0);
      $CP=$154;
      var $155=HEAP32[(($154)>>2)];
      var $156=(($155)|(0))!=0;
      var $158 = $156;label = 25; break;
    case 25: 
      var $158;
      if ($158) { label = 26; break; } else { label = 27; break; }
    case 26: 
      var $160=$CP;
      $RP=$160;
      var $161=HEAP32[(($160)>>2)];
      $R=$161;
      label = 23; break;
    case 27: 
      var $163=$RP;
      var $164=$163;
      var $165=$1;
      var $166=(($165+16)|0);
      var $167=HEAP32[(($166)>>2)];
      var $168=(($164)>>>(0)) >= (($167)>>>(0));
      var $169=(($168)&(1));
      var $170=($169);
      var $171=(($170)|(0))!=0;
      if ($171) { label = 28; break; } else { label = 29; break; }
    case 28: 
      var $173=$RP;
      HEAP32[(($173)>>2)]=0;
      label = 30; break;
    case 29: 
      _abort();
      throw "Reached an unreachable!";
    case 30: 
      label = 31; break;
    case 31: 
      label = 32; break;
    case 32: 
      var $178=$XP;
      var $179=(($178)|(0))!=0;
      if ($179) { label = 33; break; } else { label = 60; break; }
    case 33: 
      var $181=$tp;
      var $182=(($181+28)|0);
      var $183=HEAP32[(($182)>>2)];
      var $184=$1;
      var $185=(($184+304)|0);
      var $186=(($185+($183<<2))|0);
      $H=$186;
      var $187=$tp;
      var $188=$H;
      var $189=HEAP32[(($188)>>2)];
      var $190=(($187)|(0))==(($189)|(0));
      if ($190) { label = 34; break; } else { label = 37; break; }
    case 34: 
      var $192=$R;
      var $193=$H;
      HEAP32[(($193)>>2)]=$192;
      var $194=(($192)|(0))==0;
      if ($194) { label = 35; break; } else { label = 36; break; }
    case 35: 
      var $196=$tp;
      var $197=(($196+28)|0);
      var $198=HEAP32[(($197)>>2)];
      var $199=1 << $198;
      var $200=$199 ^ -1;
      var $201=$1;
      var $202=(($201+4)|0);
      var $203=HEAP32[(($202)>>2)];
      var $204=$203 & $200;
      HEAP32[(($202)>>2)]=$204;
      label = 36; break;
    case 36: 
      label = 44; break;
    case 37: 
      var $207=$XP;
      var $208=$207;
      var $209=$1;
      var $210=(($209+16)|0);
      var $211=HEAP32[(($210)>>2)];
      var $212=(($208)>>>(0)) >= (($211)>>>(0));
      var $213=(($212)&(1));
      var $214=($213);
      var $215=(($214)|(0))!=0;
      if ($215) { label = 38; break; } else { label = 42; break; }
    case 38: 
      var $217=$XP;
      var $218=(($217+16)|0);
      var $219=(($218)|0);
      var $220=HEAP32[(($219)>>2)];
      var $221=$tp;
      var $222=(($220)|(0))==(($221)|(0));
      if ($222) { label = 39; break; } else { label = 40; break; }
    case 39: 
      var $224=$R;
      var $225=$XP;
      var $226=(($225+16)|0);
      var $227=(($226)|0);
      HEAP32[(($227)>>2)]=$224;
      label = 41; break;
    case 40: 
      var $229=$R;
      var $230=$XP;
      var $231=(($230+16)|0);
      var $232=(($231+4)|0);
      HEAP32[(($232)>>2)]=$229;
      label = 41; break;
    case 41: 
      label = 43; break;
    case 42: 
      _abort();
      throw "Reached an unreachable!";
    case 43: 
      label = 44; break;
    case 44: 
      var $237=$R;
      var $238=(($237)|(0))!=0;
      if ($238) { label = 45; break; } else { label = 59; break; }
    case 45: 
      var $240=$R;
      var $241=$240;
      var $242=$1;
      var $243=(($242+16)|0);
      var $244=HEAP32[(($243)>>2)];
      var $245=(($241)>>>(0)) >= (($244)>>>(0));
      var $246=(($245)&(1));
      var $247=($246);
      var $248=(($247)|(0))!=0;
      if ($248) { label = 46; break; } else { label = 57; break; }
    case 46: 
      var $250=$XP;
      var $251=$R;
      var $252=(($251+24)|0);
      HEAP32[(($252)>>2)]=$250;
      var $253=$tp;
      var $254=(($253+16)|0);
      var $255=(($254)|0);
      var $256=HEAP32[(($255)>>2)];
      $C0=$256;
      var $257=(($256)|(0))!=0;
      if ($257) { label = 47; break; } else { label = 51; break; }
    case 47: 
      var $259=$C0;
      var $260=$259;
      var $261=$1;
      var $262=(($261+16)|0);
      var $263=HEAP32[(($262)>>2)];
      var $264=(($260)>>>(0)) >= (($263)>>>(0));
      var $265=(($264)&(1));
      var $266=($265);
      var $267=(($266)|(0))!=0;
      if ($267) { label = 48; break; } else { label = 49; break; }
    case 48: 
      var $269=$C0;
      var $270=$R;
      var $271=(($270+16)|0);
      var $272=(($271)|0);
      HEAP32[(($272)>>2)]=$269;
      var $273=$R;
      var $274=$C0;
      var $275=(($274+24)|0);
      HEAP32[(($275)>>2)]=$273;
      label = 50; break;
    case 49: 
      _abort();
      throw "Reached an unreachable!";
    case 50: 
      label = 51; break;
    case 51: 
      var $279=$tp;
      var $280=(($279+16)|0);
      var $281=(($280+4)|0);
      var $282=HEAP32[(($281)>>2)];
      $C1=$282;
      var $283=(($282)|(0))!=0;
      if ($283) { label = 52; break; } else { label = 56; break; }
    case 52: 
      var $285=$C1;
      var $286=$285;
      var $287=$1;
      var $288=(($287+16)|0);
      var $289=HEAP32[(($288)>>2)];
      var $290=(($286)>>>(0)) >= (($289)>>>(0));
      var $291=(($290)&(1));
      var $292=($291);
      var $293=(($292)|(0))!=0;
      if ($293) { label = 53; break; } else { label = 54; break; }
    case 53: 
      var $295=$C1;
      var $296=$R;
      var $297=(($296+16)|0);
      var $298=(($297+4)|0);
      HEAP32[(($298)>>2)]=$295;
      var $299=$R;
      var $300=$C1;
      var $301=(($300+24)|0);
      HEAP32[(($301)>>2)]=$299;
      label = 55; break;
    case 54: 
      _abort();
      throw "Reached an unreachable!";
    case 55: 
      label = 56; break;
    case 56: 
      label = 58; break;
    case 57: 
      _abort();
      throw "Reached an unreachable!";
    case 58: 
      label = 59; break;
    case 59: 
      label = 60; break;
    case 60: 
      label = 61; break;
    case 61: 
      var $310=$psize;
      var $311=$310 >>> 8;
      $X=$311;
      var $312=$X;
      var $313=(($312)|(0))==0;
      if ($313) { label = 62; break; } else { label = 63; break; }
    case 62: 
      $I=0;
      label = 67; break;
    case 63: 
      var $316=$X;
      var $317=(($316)>>>(0)) > 65535;
      if ($317) { label = 64; break; } else { label = 65; break; }
    case 64: 
      $I=31;
      label = 66; break;
    case 65: 
      var $320=$X;
      $Y=$320;
      var $321=$Y;
      var $322=((($321)-(256))|0);
      var $323=$322 >>> 16;
      var $324=$323 & 8;
      $N=$324;
      var $325=$N;
      var $326=$Y;
      var $327=$326 << $325;
      $Y=$327;
      var $328=((($327)-(4096))|0);
      var $329=$328 >>> 16;
      var $330=$329 & 4;
      $K=$330;
      var $331=$K;
      var $332=$N;
      var $333=((($332)+($331))|0);
      $N=$333;
      var $334=$K;
      var $335=$Y;
      var $336=$335 << $334;
      $Y=$336;
      var $337=((($336)-(16384))|0);
      var $338=$337 >>> 16;
      var $339=$338 & 2;
      $K=$339;
      var $340=$N;
      var $341=((($340)+($339))|0);
      $N=$341;
      var $342=$N;
      var $343=(((14)-($342))|0);
      var $344=$K;
      var $345=$Y;
      var $346=$345 << $344;
      $Y=$346;
      var $347=$346 >>> 15;
      var $348=((($343)+($347))|0);
      $K=$348;
      var $349=$K;
      var $350=$349 << 1;
      var $351=$psize;
      var $352=$K;
      var $353=((($352)+(7))|0);
      var $354=$351 >>> (($353)>>>(0));
      var $355=$354 & 1;
      var $356=((($350)+($355))|0);
      $I=$356;
      label = 66; break;
    case 66: 
      label = 67; break;
    case 67: 
      var $359=$I;
      var $360=$1;
      var $361=(($360+304)|0);
      var $362=(($361+($359<<2))|0);
      $H1=$362;
      var $363=$I;
      var $364=$tp;
      var $365=(($364+28)|0);
      HEAP32[(($365)>>2)]=$363;
      var $366=$tp;
      var $367=(($366+16)|0);
      var $368=(($367+4)|0);
      HEAP32[(($368)>>2)]=0;
      var $369=$tp;
      var $370=(($369+16)|0);
      var $371=(($370)|0);
      HEAP32[(($371)>>2)]=0;
      var $372=$1;
      var $373=(($372+4)|0);
      var $374=HEAP32[(($373)>>2)];
      var $375=$I;
      var $376=1 << $375;
      var $377=$374 & $376;
      var $378=(($377)|(0))!=0;
      if ($378) { label = 69; break; } else { label = 68; break; }
    case 68: 
      var $380=$I;
      var $381=1 << $380;
      var $382=$1;
      var $383=(($382+4)|0);
      var $384=HEAP32[(($383)>>2)];
      var $385=$384 | $381;
      HEAP32[(($383)>>2)]=$385;
      var $386=$tp;
      var $387=$H1;
      HEAP32[(($387)>>2)]=$386;
      var $388=$H1;
      var $389=$388;
      var $390=$tp;
      var $391=(($390+24)|0);
      HEAP32[(($391)>>2)]=$389;
      var $392=$tp;
      var $393=$tp;
      var $394=(($393+12)|0);
      HEAP32[(($394)>>2)]=$392;
      var $395=$tp;
      var $396=(($395+8)|0);
      HEAP32[(($396)>>2)]=$392;
      label = 87; break;
    case 69: 
      var $398=$H1;
      var $399=HEAP32[(($398)>>2)];
      $T=$399;
      var $400=$psize;
      var $401=$I;
      var $402=(($401)|(0))==31;
      if ($402) { label = 70; break; } else { label = 71; break; }
    case 70: 
      var $411 = 0;label = 72; break;
    case 71: 
      var $405=$I;
      var $406=$405 >>> 1;
      var $407=((($406)+(8))|0);
      var $408=((($407)-(2))|0);
      var $409=(((31)-($408))|0);
      var $411 = $409;label = 72; break;
    case 72: 
      var $411;
      var $412=$400 << $411;
      $K2=$412;
      label = 73; break;
    case 73: 
      var $414=$T;
      var $415=(($414+4)|0);
      var $416=HEAP32[(($415)>>2)];
      var $417=$416 & -8;
      var $418=$psize;
      var $419=(($417)|(0))!=(($418)|(0));
      if ($419) { label = 74; break; } else { label = 80; break; }
    case 74: 
      var $421=$K2;
      var $422=$421 >>> 31;
      var $423=$422 & 1;
      var $424=$T;
      var $425=(($424+16)|0);
      var $426=(($425+($423<<2))|0);
      $C=$426;
      var $427=$K2;
      var $428=$427 << 1;
      $K2=$428;
      var $429=$C;
      var $430=HEAP32[(($429)>>2)];
      var $431=(($430)|(0))!=0;
      if ($431) { label = 75; break; } else { label = 76; break; }
    case 75: 
      var $433=$C;
      var $434=HEAP32[(($433)>>2)];
      $T=$434;
      label = 79; break;
    case 76: 
      var $436=$C;
      var $437=$436;
      var $438=$1;
      var $439=(($438+16)|0);
      var $440=HEAP32[(($439)>>2)];
      var $441=(($437)>>>(0)) >= (($440)>>>(0));
      var $442=(($441)&(1));
      var $443=($442);
      var $444=(($443)|(0))!=0;
      if ($444) { label = 77; break; } else { label = 78; break; }
    case 77: 
      var $446=$tp;
      var $447=$C;
      HEAP32[(($447)>>2)]=$446;
      var $448=$T;
      var $449=$tp;
      var $450=(($449+24)|0);
      HEAP32[(($450)>>2)]=$448;
      var $451=$tp;
      var $452=$tp;
      var $453=(($452+12)|0);
      HEAP32[(($453)>>2)]=$451;
      var $454=$tp;
      var $455=(($454+8)|0);
      HEAP32[(($455)>>2)]=$451;
      label = 86; break;
    case 78: 
      _abort();
      throw "Reached an unreachable!";
    case 79: 
      label = 85; break;
    case 80: 
      var $459=$T;
      var $460=(($459+8)|0);
      var $461=HEAP32[(($460)>>2)];
      $F3=$461;
      var $462=$T;
      var $463=$462;
      var $464=$1;
      var $465=(($464+16)|0);
      var $466=HEAP32[(($465)>>2)];
      var $467=(($463)>>>(0)) >= (($466)>>>(0));
      if ($467) { label = 81; break; } else { var $476 = 0;label = 82; break; }
    case 81: 
      var $469=$F3;
      var $470=$469;
      var $471=$1;
      var $472=(($471+16)|0);
      var $473=HEAP32[(($472)>>2)];
      var $474=(($470)>>>(0)) >= (($473)>>>(0));
      var $476 = $474;label = 82; break;
    case 82: 
      var $476;
      var $477=(($476)&(1));
      var $478=($477);
      var $479=(($478)|(0))!=0;
      if ($479) { label = 83; break; } else { label = 84; break; }
    case 83: 
      var $481=$tp;
      var $482=$F3;
      var $483=(($482+12)|0);
      HEAP32[(($483)>>2)]=$481;
      var $484=$T;
      var $485=(($484+8)|0);
      HEAP32[(($485)>>2)]=$481;
      var $486=$F3;
      var $487=$tp;
      var $488=(($487+8)|0);
      HEAP32[(($488)>>2)]=$486;
      var $489=$T;
      var $490=$tp;
      var $491=(($490+12)|0);
      HEAP32[(($491)>>2)]=$489;
      var $492=$tp;
      var $493=(($492+24)|0);
      HEAP32[(($493)>>2)]=0;
      label = 86; break;
    case 84: 
      _abort();
      throw "Reached an unreachable!";
    case 85: 
      label = 73; break;
    case 86: 
      label = 87; break;
    case 87: 
      label = 88; break;
    case 88: 
      label = 89; break;
    case 89: 
      var $500=$sp;
      $pred=$500;
      var $501=$next;
      $sp=$501;
      label = 2; break;
    case 90: 
      var $503=$nsegs;
      var $504=(($503)>>>(0)) > 4294967295;
      if ($504) { label = 91; break; } else { label = 92; break; }
    case 91: 
      var $506=$nsegs;
      var $509 = $506;label = 93; break;
    case 92: 
      var $509 = -1;label = 93; break;
    case 93: 
      var $509;
      var $510=$1;
      var $511=(($510+32)|0);
      HEAP32[(($511)>>2)]=$509;
      var $512=$released;
      return $512;
    default: assert(0, "bad label: " + label);
  }
}
function _init_mparams() {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $magic;
      var $psize;
      var $gsize;
      var $1=HEAP32[((((56)|0))>>2)];
      var $2=(($1)|(0))==0;
      if ($2) { label = 2; break; } else { label = 6; break; }
    case 2: 
      var $4=_sysconf(8);
      $psize=$4;
      var $5=$psize;
      $gsize=$5;
      var $6=$gsize;
      var $7=$gsize;
      var $8=((($7)-(1))|0);
      var $9=$6 & $8;
      var $10=(($9)|(0))!=0;
      if ($10) { label = 4; break; } else { label = 3; break; }
    case 3: 
      var $12=$psize;
      var $13=$psize;
      var $14=((($13)-(1))|0);
      var $15=$12 & $14;
      var $16=(($15)|(0))!=0;
      if ($16) { label = 4; break; } else { label = 5; break; }
    case 4: 
      _abort();
      throw "Reached an unreachable!";
    case 5: 
      var $19=$gsize;
      HEAP32[((((64)|0))>>2)]=$19;
      var $20=$psize;
      HEAP32[((((60)|0))>>2)]=$20;
      HEAP32[((((68)|0))>>2)]=-1;
      HEAP32[((((72)|0))>>2)]=2097152;
      HEAP32[((((76)|0))>>2)]=0;
      var $21=HEAP32[((((76)|0))>>2)];
      HEAP32[((((852)|0))>>2)]=$21;
      var $22=_time(0);
      var $23=$22 ^ 1431655765;
      $magic=$23;
      var $24=$magic;
      var $25=$24 | 8;
      $magic=$25;
      var $26=$magic;
      var $27=$26 & -8;
      $magic=$27;
      var $28=$magic;
      HEAP32[((((56)|0))>>2)]=$28;
      label = 6; break;
    case 6: 
      return 1;
    default: assert(0, "bad label: " + label);
  }
}
function _mmap_alloc($m, $nb) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $mmsize;
      var $fp;
      var $mm;
      var $offset;
      var $psize;
      var $p;
      $2=$m;
      $3=$nb;
      var $4=$3;
      var $5=((($4)+(24))|0);
      var $6=((($5)+(7))|0);
      var $7=HEAP32[((((60)|0))>>2)];
      var $8=((($7)-(1))|0);
      var $9=((($6)+($8))|0);
      var $10=HEAP32[((((60)|0))>>2)];
      var $11=((($10)-(1))|0);
      var $12=$11 ^ -1;
      var $13=$9 & $12;
      $mmsize=$13;
      var $14=$2;
      var $15=(($14+440)|0);
      var $16=HEAP32[(($15)>>2)];
      var $17=(($16)|(0))!=0;
      if ($17) { label = 2; break; } else { label = 6; break; }
    case 2: 
      var $19=$2;
      var $20=(($19+432)|0);
      var $21=HEAP32[(($20)>>2)];
      var $22=$mmsize;
      var $23=((($21)+($22))|0);
      $fp=$23;
      var $24=$fp;
      var $25=$2;
      var $26=(($25+432)|0);
      var $27=HEAP32[(($26)>>2)];
      var $28=(($24)>>>(0)) <= (($27)>>>(0));
      if ($28) { label = 4; break; } else { label = 3; break; }
    case 3: 
      var $30=$fp;
      var $31=$2;
      var $32=(($31+440)|0);
      var $33=HEAP32[(($32)>>2)];
      var $34=(($30)>>>(0)) > (($33)>>>(0));
      if ($34) { label = 4; break; } else { label = 5; break; }
    case 4: 
      $1=0;
      label = 19; break;
    case 5: 
      label = 6; break;
    case 6: 
      var $38=$mmsize;
      var $39=$3;
      var $40=(($38)>>>(0)) > (($39)>>>(0));
      if ($40) { label = 7; break; } else { label = 18; break; }
    case 7: 
      $mm=-1;
      var $42=$mm;
      var $43=(($42)|(0))!=-1;
      if ($43) { label = 8; break; } else { label = 17; break; }
    case 8: 
      var $45=$mm;
      var $46=(($45+8)|0);
      var $47=$46;
      var $48=$47 & 7;
      var $49=(($48)|(0))==0;
      if ($49) { label = 9; break; } else { label = 10; break; }
    case 9: 
      var $59 = 0;label = 11; break;
    case 10: 
      var $52=$mm;
      var $53=(($52+8)|0);
      var $54=$53;
      var $55=$54 & 7;
      var $56=(((8)-($55))|0);
      var $57=$56 & 7;
      var $59 = $57;label = 11; break;
    case 11: 
      var $59;
      $offset=$59;
      var $60=$mmsize;
      var $61=$offset;
      var $62=((($60)-($61))|0);
      var $63=((($62)-(16))|0);
      $psize=$63;
      var $64=$mm;
      var $65=$offset;
      var $66=(($64+$65)|0);
      var $67=$66;
      $p=$67;
      var $68=$offset;
      var $69=$p;
      var $70=(($69)|0);
      HEAP32[(($70)>>2)]=$68;
      var $71=$psize;
      var $72=$p;
      var $73=(($72+4)|0);
      HEAP32[(($73)>>2)]=$71;
      var $74=$p;
      var $75=$74;
      var $76=$psize;
      var $77=(($75+$76)|0);
      var $78=$77;
      var $79=(($78+4)|0);
      HEAP32[(($79)>>2)]=7;
      var $80=$p;
      var $81=$80;
      var $82=$psize;
      var $83=((($82)+(4))|0);
      var $84=(($81+$83)|0);
      var $85=$84;
      var $86=(($85+4)|0);
      HEAP32[(($86)>>2)]=0;
      var $87=$2;
      var $88=(($87+16)|0);
      var $89=HEAP32[(($88)>>2)];
      var $90=(($89)|(0))==0;
      if ($90) { label = 13; break; } else { label = 12; break; }
    case 12: 
      var $92=$mm;
      var $93=$2;
      var $94=(($93+16)|0);
      var $95=HEAP32[(($94)>>2)];
      var $96=(($92)>>>(0)) < (($95)>>>(0));
      if ($96) { label = 13; break; } else { label = 14; break; }
    case 13: 
      var $98=$mm;
      var $99=$2;
      var $100=(($99+16)|0);
      HEAP32[(($100)>>2)]=$98;
      label = 14; break;
    case 14: 
      var $102=$mmsize;
      var $103=$2;
      var $104=(($103+432)|0);
      var $105=HEAP32[(($104)>>2)];
      var $106=((($105)+($102))|0);
      HEAP32[(($104)>>2)]=$106;
      var $107=$2;
      var $108=(($107+436)|0);
      var $109=HEAP32[(($108)>>2)];
      var $110=(($106)>>>(0)) > (($109)>>>(0));
      if ($110) { label = 15; break; } else { label = 16; break; }
    case 15: 
      var $112=$2;
      var $113=(($112+432)|0);
      var $114=HEAP32[(($113)>>2)];
      var $115=$2;
      var $116=(($115+436)|0);
      HEAP32[(($116)>>2)]=$114;
      label = 16; break;
    case 16: 
      var $118=$p;
      var $119=$118;
      var $120=(($119+8)|0);
      $1=$120;
      label = 19; break;
    case 17: 
      label = 18; break;
    case 18: 
      $1=0;
      label = 19; break;
    case 19: 
      var $124=$1;
      return $124;
    default: assert(0, "bad label: " + label);
  }
}
function _init_bins($m) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $i;
      var $bin;
      $1=$m;
      $i=0;
      label = 2; break;
    case 2: 
      var $3=$i;
      var $4=(($3)>>>(0)) < 32;
      if ($4) { label = 3; break; } else { label = 5; break; }
    case 3: 
      var $6=$i;
      var $7=$6 << 1;
      var $8=$1;
      var $9=(($8+40)|0);
      var $10=(($9+($7<<2))|0);
      var $11=$10;
      var $12=$11;
      $bin=$12;
      var $13=$bin;
      var $14=$bin;
      var $15=(($14+12)|0);
      HEAP32[(($15)>>2)]=$13;
      var $16=$bin;
      var $17=(($16+8)|0);
      HEAP32[(($17)>>2)]=$13;
      label = 4; break;
    case 4: 
      var $19=$i;
      var $20=((($19)+(1))|0);
      $i=$20;
      label = 2; break;
    case 5: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function _prepend_alloc($m, $newbase, $oldbase, $nb) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $p;
      var $oldfirst;
      var $psize;
      var $q;
      var $qsize;
      var $tsize;
      var $dsize;
      var $nsize;
      var $F;
      var $B;
      var $I;
      var $TP;
      var $XP;
      var $R;
      var $F1;
      var $RP;
      var $CP;
      var $H;
      var $C0;
      var $C1;
      var $I2;
      var $B3;
      var $F4;
      var $TP5;
      var $H6;
      var $I7;
      var $X;
      var $Y;
      var $N;
      var $K;
      var $T;
      var $K8;
      var $C;
      var $F9;
      $1=$m;
      $2=$newbase;
      $3=$oldbase;
      $4=$nb;
      var $5=$2;
      var $6=$2;
      var $7=(($6+8)|0);
      var $8=$7;
      var $9=$8 & 7;
      var $10=(($9)|(0))==0;
      if ($10) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $20 = 0;label = 4; break;
    case 3: 
      var $13=$2;
      var $14=(($13+8)|0);
      var $15=$14;
      var $16=$15 & 7;
      var $17=(((8)-($16))|0);
      var $18=$17 & 7;
      var $20 = $18;label = 4; break;
    case 4: 
      var $20;
      var $21=(($5+$20)|0);
      var $22=$21;
      $p=$22;
      var $23=$3;
      var $24=$3;
      var $25=(($24+8)|0);
      var $26=$25;
      var $27=$26 & 7;
      var $28=(($27)|(0))==0;
      if ($28) { label = 5; break; } else { label = 6; break; }
    case 5: 
      var $38 = 0;label = 7; break;
    case 6: 
      var $31=$3;
      var $32=(($31+8)|0);
      var $33=$32;
      var $34=$33 & 7;
      var $35=(((8)-($34))|0);
      var $36=$35 & 7;
      var $38 = $36;label = 7; break;
    case 7: 
      var $38;
      var $39=(($23+$38)|0);
      var $40=$39;
      $oldfirst=$40;
      var $41=$oldfirst;
      var $42=$41;
      var $43=$p;
      var $44=$43;
      var $45=$42;
      var $46=$44;
      var $47=((($45)-($46))|0);
      $psize=$47;
      var $48=$p;
      var $49=$48;
      var $50=$4;
      var $51=(($49+$50)|0);
      var $52=$51;
      $q=$52;
      var $53=$psize;
      var $54=$4;
      var $55=((($53)-($54))|0);
      $qsize=$55;
      var $56=$4;
      var $57=$56 | 1;
      var $58=$57 | 2;
      var $59=$p;
      var $60=(($59+4)|0);
      HEAP32[(($60)>>2)]=$58;
      var $61=$oldfirst;
      var $62=$1;
      var $63=(($62+24)|0);
      var $64=HEAP32[(($63)>>2)];
      var $65=(($61)|(0))==(($64)|(0));
      if ($65) { label = 8; break; } else { label = 9; break; }
    case 8: 
      var $67=$qsize;
      var $68=$1;
      var $69=(($68+12)|0);
      var $70=HEAP32[(($69)>>2)];
      var $71=((($70)+($67))|0);
      HEAP32[(($69)>>2)]=$71;
      $tsize=$71;
      var $72=$q;
      var $73=$1;
      var $74=(($73+24)|0);
      HEAP32[(($74)>>2)]=$72;
      var $75=$tsize;
      var $76=$75 | 1;
      var $77=$q;
      var $78=(($77+4)|0);
      HEAP32[(($78)>>2)]=$76;
      label = 118; break;
    case 9: 
      var $80=$oldfirst;
      var $81=$1;
      var $82=(($81+20)|0);
      var $83=HEAP32[(($82)>>2)];
      var $84=(($80)|(0))==(($83)|(0));
      if ($84) { label = 10; break; } else { label = 11; break; }
    case 10: 
      var $86=$qsize;
      var $87=$1;
      var $88=(($87+8)|0);
      var $89=HEAP32[(($88)>>2)];
      var $90=((($89)+($86))|0);
      HEAP32[(($88)>>2)]=$90;
      $dsize=$90;
      var $91=$q;
      var $92=$1;
      var $93=(($92+20)|0);
      HEAP32[(($93)>>2)]=$91;
      var $94=$dsize;
      var $95=$94 | 1;
      var $96=$q;
      var $97=(($96+4)|0);
      HEAP32[(($97)>>2)]=$95;
      var $98=$dsize;
      var $99=$q;
      var $100=$99;
      var $101=$dsize;
      var $102=(($100+$101)|0);
      var $103=$102;
      var $104=(($103)|0);
      HEAP32[(($104)>>2)]=$98;
      label = 117; break;
    case 11: 
      var $106=$oldfirst;
      var $107=(($106+4)|0);
      var $108=HEAP32[(($107)>>2)];
      var $109=$108 & 3;
      var $110=(($109)|(0))!=1;
      if ($110) { label = 81; break; } else { label = 12; break; }
    case 12: 
      var $112=$oldfirst;
      var $113=(($112+4)|0);
      var $114=HEAP32[(($113)>>2)];
      var $115=$114 & -8;
      $nsize=$115;
      var $116=$nsize;
      var $117=$116 >>> 3;
      var $118=(($117)>>>(0)) < 32;
      if ($118) { label = 13; break; } else { label = 31; break; }
    case 13: 
      var $120=$oldfirst;
      var $121=(($120+8)|0);
      var $122=HEAP32[(($121)>>2)];
      $F=$122;
      var $123=$oldfirst;
      var $124=(($123+12)|0);
      var $125=HEAP32[(($124)>>2)];
      $B=$125;
      var $126=$nsize;
      var $127=$126 >>> 3;
      $I=$127;
      var $128=$F;
      var $129=$I;
      var $130=$129 << 1;
      var $131=$1;
      var $132=(($131+40)|0);
      var $133=(($132+($130<<2))|0);
      var $134=$133;
      var $135=$134;
      var $136=(($128)|(0))==(($135)|(0));
      if ($136) { var $153 = 1;label = 17; break; } else { label = 14; break; }
    case 14: 
      var $138=$F;
      var $139=$138;
      var $140=$1;
      var $141=(($140+16)|0);
      var $142=HEAP32[(($141)>>2)];
      var $143=(($139)>>>(0)) >= (($142)>>>(0));
      if ($143) { label = 15; break; } else { var $151 = 0;label = 16; break; }
    case 15: 
      var $145=$F;
      var $146=(($145+12)|0);
      var $147=HEAP32[(($146)>>2)];
      var $148=$oldfirst;
      var $149=(($147)|(0))==(($148)|(0));
      var $151 = $149;label = 16; break;
    case 16: 
      var $151;
      var $153 = $151;label = 17; break;
    case 17: 
      var $153;
      var $154=(($153)&(1));
      var $155=($154);
      var $156=(($155)|(0))!=0;
      if ($156) { label = 18; break; } else { label = 29; break; }
    case 18: 
      var $158=$B;
      var $159=$F;
      var $160=(($158)|(0))==(($159)|(0));
      if ($160) { label = 19; break; } else { label = 20; break; }
    case 19: 
      var $162=$I;
      var $163=1 << $162;
      var $164=$163 ^ -1;
      var $165=$1;
      var $166=(($165)|0);
      var $167=HEAP32[(($166)>>2)];
      var $168=$167 & $164;
      HEAP32[(($166)>>2)]=$168;
      label = 28; break;
    case 20: 
      var $170=$B;
      var $171=$I;
      var $172=$171 << 1;
      var $173=$1;
      var $174=(($173+40)|0);
      var $175=(($174+($172<<2))|0);
      var $176=$175;
      var $177=$176;
      var $178=(($170)|(0))==(($177)|(0));
      if ($178) { var $195 = 1;label = 24; break; } else { label = 21; break; }
    case 21: 
      var $180=$B;
      var $181=$180;
      var $182=$1;
      var $183=(($182+16)|0);
      var $184=HEAP32[(($183)>>2)];
      var $185=(($181)>>>(0)) >= (($184)>>>(0));
      if ($185) { label = 22; break; } else { var $193 = 0;label = 23; break; }
    case 22: 
      var $187=$B;
      var $188=(($187+8)|0);
      var $189=HEAP32[(($188)>>2)];
      var $190=$oldfirst;
      var $191=(($189)|(0))==(($190)|(0));
      var $193 = $191;label = 23; break;
    case 23: 
      var $193;
      var $195 = $193;label = 24; break;
    case 24: 
      var $195;
      var $196=(($195)&(1));
      var $197=($196);
      var $198=(($197)|(0))!=0;
      if ($198) { label = 25; break; } else { label = 26; break; }
    case 25: 
      var $200=$B;
      var $201=$F;
      var $202=(($201+12)|0);
      HEAP32[(($202)>>2)]=$200;
      var $203=$F;
      var $204=$B;
      var $205=(($204+8)|0);
      HEAP32[(($205)>>2)]=$203;
      label = 27; break;
    case 26: 
      _abort();
      throw "Reached an unreachable!";
    case 27: 
      label = 28; break;
    case 28: 
      label = 30; break;
    case 29: 
      _abort();
      throw "Reached an unreachable!";
    case 30: 
      label = 80; break;
    case 31: 
      var $212=$oldfirst;
      var $213=$212;
      $TP=$213;
      var $214=$TP;
      var $215=(($214+24)|0);
      var $216=HEAP32[(($215)>>2)];
      $XP=$216;
      var $217=$TP;
      var $218=(($217+12)|0);
      var $219=HEAP32[(($218)>>2)];
      var $220=$TP;
      var $221=(($219)|(0))!=(($220)|(0));
      if ($221) { label = 32; break; } else { label = 39; break; }
    case 32: 
      var $223=$TP;
      var $224=(($223+8)|0);
      var $225=HEAP32[(($224)>>2)];
      $F1=$225;
      var $226=$TP;
      var $227=(($226+12)|0);
      var $228=HEAP32[(($227)>>2)];
      $R=$228;
      var $229=$F1;
      var $230=$229;
      var $231=$1;
      var $232=(($231+16)|0);
      var $233=HEAP32[(($232)>>2)];
      var $234=(($230)>>>(0)) >= (($233)>>>(0));
      if ($234) { label = 33; break; } else { var $248 = 0;label = 35; break; }
    case 33: 
      var $236=$F1;
      var $237=(($236+12)|0);
      var $238=HEAP32[(($237)>>2)];
      var $239=$TP;
      var $240=(($238)|(0))==(($239)|(0));
      if ($240) { label = 34; break; } else { var $248 = 0;label = 35; break; }
    case 34: 
      var $242=$R;
      var $243=(($242+8)|0);
      var $244=HEAP32[(($243)>>2)];
      var $245=$TP;
      var $246=(($244)|(0))==(($245)|(0));
      var $248 = $246;label = 35; break;
    case 35: 
      var $248;
      var $249=(($248)&(1));
      var $250=($249);
      var $251=(($250)|(0))!=0;
      if ($251) { label = 36; break; } else { label = 37; break; }
    case 36: 
      var $253=$R;
      var $254=$F1;
      var $255=(($254+12)|0);
      HEAP32[(($255)>>2)]=$253;
      var $256=$F1;
      var $257=$R;
      var $258=(($257+8)|0);
      HEAP32[(($258)>>2)]=$256;
      label = 38; break;
    case 37: 
      _abort();
      throw "Reached an unreachable!";
    case 38: 
      label = 51; break;
    case 39: 
      var $262=$TP;
      var $263=(($262+16)|0);
      var $264=(($263+4)|0);
      $RP=$264;
      var $265=HEAP32[(($264)>>2)];
      $R=$265;
      var $266=(($265)|(0))!=0;
      if ($266) { label = 41; break; } else { label = 40; break; }
    case 40: 
      var $268=$TP;
      var $269=(($268+16)|0);
      var $270=(($269)|0);
      $RP=$270;
      var $271=HEAP32[(($270)>>2)];
      $R=$271;
      var $272=(($271)|(0))!=0;
      if ($272) { label = 41; break; } else { label = 50; break; }
    case 41: 
      label = 42; break;
    case 42: 
      var $275=$R;
      var $276=(($275+16)|0);
      var $277=(($276+4)|0);
      $CP=$277;
      var $278=HEAP32[(($277)>>2)];
      var $279=(($278)|(0))!=0;
      if ($279) { var $287 = 1;label = 44; break; } else { label = 43; break; }
    case 43: 
      var $281=$R;
      var $282=(($281+16)|0);
      var $283=(($282)|0);
      $CP=$283;
      var $284=HEAP32[(($283)>>2)];
      var $285=(($284)|(0))!=0;
      var $287 = $285;label = 44; break;
    case 44: 
      var $287;
      if ($287) { label = 45; break; } else { label = 46; break; }
    case 45: 
      var $289=$CP;
      $RP=$289;
      var $290=HEAP32[(($289)>>2)];
      $R=$290;
      label = 42; break;
    case 46: 
      var $292=$RP;
      var $293=$292;
      var $294=$1;
      var $295=(($294+16)|0);
      var $296=HEAP32[(($295)>>2)];
      var $297=(($293)>>>(0)) >= (($296)>>>(0));
      var $298=(($297)&(1));
      var $299=($298);
      var $300=(($299)|(0))!=0;
      if ($300) { label = 47; break; } else { label = 48; break; }
    case 47: 
      var $302=$RP;
      HEAP32[(($302)>>2)]=0;
      label = 49; break;
    case 48: 
      _abort();
      throw "Reached an unreachable!";
    case 49: 
      label = 50; break;
    case 50: 
      label = 51; break;
    case 51: 
      var $307=$XP;
      var $308=(($307)|(0))!=0;
      if ($308) { label = 52; break; } else { label = 79; break; }
    case 52: 
      var $310=$TP;
      var $311=(($310+28)|0);
      var $312=HEAP32[(($311)>>2)];
      var $313=$1;
      var $314=(($313+304)|0);
      var $315=(($314+($312<<2))|0);
      $H=$315;
      var $316=$TP;
      var $317=$H;
      var $318=HEAP32[(($317)>>2)];
      var $319=(($316)|(0))==(($318)|(0));
      if ($319) { label = 53; break; } else { label = 56; break; }
    case 53: 
      var $321=$R;
      var $322=$H;
      HEAP32[(($322)>>2)]=$321;
      var $323=(($321)|(0))==0;
      if ($323) { label = 54; break; } else { label = 55; break; }
    case 54: 
      var $325=$TP;
      var $326=(($325+28)|0);
      var $327=HEAP32[(($326)>>2)];
      var $328=1 << $327;
      var $329=$328 ^ -1;
      var $330=$1;
      var $331=(($330+4)|0);
      var $332=HEAP32[(($331)>>2)];
      var $333=$332 & $329;
      HEAP32[(($331)>>2)]=$333;
      label = 55; break;
    case 55: 
      label = 63; break;
    case 56: 
      var $336=$XP;
      var $337=$336;
      var $338=$1;
      var $339=(($338+16)|0);
      var $340=HEAP32[(($339)>>2)];
      var $341=(($337)>>>(0)) >= (($340)>>>(0));
      var $342=(($341)&(1));
      var $343=($342);
      var $344=(($343)|(0))!=0;
      if ($344) { label = 57; break; } else { label = 61; break; }
    case 57: 
      var $346=$XP;
      var $347=(($346+16)|0);
      var $348=(($347)|0);
      var $349=HEAP32[(($348)>>2)];
      var $350=$TP;
      var $351=(($349)|(0))==(($350)|(0));
      if ($351) { label = 58; break; } else { label = 59; break; }
    case 58: 
      var $353=$R;
      var $354=$XP;
      var $355=(($354+16)|0);
      var $356=(($355)|0);
      HEAP32[(($356)>>2)]=$353;
      label = 60; break;
    case 59: 
      var $358=$R;
      var $359=$XP;
      var $360=(($359+16)|0);
      var $361=(($360+4)|0);
      HEAP32[(($361)>>2)]=$358;
      label = 60; break;
    case 60: 
      label = 62; break;
    case 61: 
      _abort();
      throw "Reached an unreachable!";
    case 62: 
      label = 63; break;
    case 63: 
      var $366=$R;
      var $367=(($366)|(0))!=0;
      if ($367) { label = 64; break; } else { label = 78; break; }
    case 64: 
      var $369=$R;
      var $370=$369;
      var $371=$1;
      var $372=(($371+16)|0);
      var $373=HEAP32[(($372)>>2)];
      var $374=(($370)>>>(0)) >= (($373)>>>(0));
      var $375=(($374)&(1));
      var $376=($375);
      var $377=(($376)|(0))!=0;
      if ($377) { label = 65; break; } else { label = 76; break; }
    case 65: 
      var $379=$XP;
      var $380=$R;
      var $381=(($380+24)|0);
      HEAP32[(($381)>>2)]=$379;
      var $382=$TP;
      var $383=(($382+16)|0);
      var $384=(($383)|0);
      var $385=HEAP32[(($384)>>2)];
      $C0=$385;
      var $386=(($385)|(0))!=0;
      if ($386) { label = 66; break; } else { label = 70; break; }
    case 66: 
      var $388=$C0;
      var $389=$388;
      var $390=$1;
      var $391=(($390+16)|0);
      var $392=HEAP32[(($391)>>2)];
      var $393=(($389)>>>(0)) >= (($392)>>>(0));
      var $394=(($393)&(1));
      var $395=($394);
      var $396=(($395)|(0))!=0;
      if ($396) { label = 67; break; } else { label = 68; break; }
    case 67: 
      var $398=$C0;
      var $399=$R;
      var $400=(($399+16)|0);
      var $401=(($400)|0);
      HEAP32[(($401)>>2)]=$398;
      var $402=$R;
      var $403=$C0;
      var $404=(($403+24)|0);
      HEAP32[(($404)>>2)]=$402;
      label = 69; break;
    case 68: 
      _abort();
      throw "Reached an unreachable!";
    case 69: 
      label = 70; break;
    case 70: 
      var $408=$TP;
      var $409=(($408+16)|0);
      var $410=(($409+4)|0);
      var $411=HEAP32[(($410)>>2)];
      $C1=$411;
      var $412=(($411)|(0))!=0;
      if ($412) { label = 71; break; } else { label = 75; break; }
    case 71: 
      var $414=$C1;
      var $415=$414;
      var $416=$1;
      var $417=(($416+16)|0);
      var $418=HEAP32[(($417)>>2)];
      var $419=(($415)>>>(0)) >= (($418)>>>(0));
      var $420=(($419)&(1));
      var $421=($420);
      var $422=(($421)|(0))!=0;
      if ($422) { label = 72; break; } else { label = 73; break; }
    case 72: 
      var $424=$C1;
      var $425=$R;
      var $426=(($425+16)|0);
      var $427=(($426+4)|0);
      HEAP32[(($427)>>2)]=$424;
      var $428=$R;
      var $429=$C1;
      var $430=(($429+24)|0);
      HEAP32[(($430)>>2)]=$428;
      label = 74; break;
    case 73: 
      _abort();
      throw "Reached an unreachable!";
    case 74: 
      label = 75; break;
    case 75: 
      label = 77; break;
    case 76: 
      _abort();
      throw "Reached an unreachable!";
    case 77: 
      label = 78; break;
    case 78: 
      label = 79; break;
    case 79: 
      label = 80; break;
    case 80: 
      var $439=$oldfirst;
      var $440=$439;
      var $441=$nsize;
      var $442=(($440+$441)|0);
      var $443=$442;
      $oldfirst=$443;
      var $444=$nsize;
      var $445=$qsize;
      var $446=((($445)+($444))|0);
      $qsize=$446;
      label = 81; break;
    case 81: 
      var $448=$oldfirst;
      var $449=(($448+4)|0);
      var $450=HEAP32[(($449)>>2)];
      var $451=$450 & -2;
      HEAP32[(($449)>>2)]=$451;
      var $452=$qsize;
      var $453=$452 | 1;
      var $454=$q;
      var $455=(($454+4)|0);
      HEAP32[(($455)>>2)]=$453;
      var $456=$qsize;
      var $457=$q;
      var $458=$457;
      var $459=$qsize;
      var $460=(($458+$459)|0);
      var $461=$460;
      var $462=(($461)|0);
      HEAP32[(($462)>>2)]=$456;
      var $463=$qsize;
      var $464=$463 >>> 3;
      var $465=(($464)>>>(0)) < 32;
      if ($465) { label = 82; break; } else { label = 89; break; }
    case 82: 
      var $467=$qsize;
      var $468=$467 >>> 3;
      $I2=$468;
      var $469=$I2;
      var $470=$469 << 1;
      var $471=$1;
      var $472=(($471+40)|0);
      var $473=(($472+($470<<2))|0);
      var $474=$473;
      var $475=$474;
      $B3=$475;
      var $476=$B3;
      $F4=$476;
      var $477=$1;
      var $478=(($477)|0);
      var $479=HEAP32[(($478)>>2)];
      var $480=$I2;
      var $481=1 << $480;
      var $482=$479 & $481;
      var $483=(($482)|(0))!=0;
      if ($483) { label = 84; break; } else { label = 83; break; }
    case 83: 
      var $485=$I2;
      var $486=1 << $485;
      var $487=$1;
      var $488=(($487)|0);
      var $489=HEAP32[(($488)>>2)];
      var $490=$489 | $486;
      HEAP32[(($488)>>2)]=$490;
      label = 88; break;
    case 84: 
      var $492=$B3;
      var $493=(($492+8)|0);
      var $494=HEAP32[(($493)>>2)];
      var $495=$494;
      var $496=$1;
      var $497=(($496+16)|0);
      var $498=HEAP32[(($497)>>2)];
      var $499=(($495)>>>(0)) >= (($498)>>>(0));
      var $500=(($499)&(1));
      var $501=($500);
      var $502=(($501)|(0))!=0;
      if ($502) { label = 85; break; } else { label = 86; break; }
    case 85: 
      var $504=$B3;
      var $505=(($504+8)|0);
      var $506=HEAP32[(($505)>>2)];
      $F4=$506;
      label = 87; break;
    case 86: 
      _abort();
      throw "Reached an unreachable!";
    case 87: 
      label = 88; break;
    case 88: 
      var $510=$q;
      var $511=$B3;
      var $512=(($511+8)|0);
      HEAP32[(($512)>>2)]=$510;
      var $513=$q;
      var $514=$F4;
      var $515=(($514+12)|0);
      HEAP32[(($515)>>2)]=$513;
      var $516=$F4;
      var $517=$q;
      var $518=(($517+8)|0);
      HEAP32[(($518)>>2)]=$516;
      var $519=$B3;
      var $520=$q;
      var $521=(($520+12)|0);
      HEAP32[(($521)>>2)]=$519;
      label = 116; break;
    case 89: 
      var $523=$q;
      var $524=$523;
      $TP5=$524;
      var $525=$qsize;
      var $526=$525 >>> 8;
      $X=$526;
      var $527=$X;
      var $528=(($527)|(0))==0;
      if ($528) { label = 90; break; } else { label = 91; break; }
    case 90: 
      $I7=0;
      label = 95; break;
    case 91: 
      var $531=$X;
      var $532=(($531)>>>(0)) > 65535;
      if ($532) { label = 92; break; } else { label = 93; break; }
    case 92: 
      $I7=31;
      label = 94; break;
    case 93: 
      var $535=$X;
      $Y=$535;
      var $536=$Y;
      var $537=((($536)-(256))|0);
      var $538=$537 >>> 16;
      var $539=$538 & 8;
      $N=$539;
      var $540=$N;
      var $541=$Y;
      var $542=$541 << $540;
      $Y=$542;
      var $543=((($542)-(4096))|0);
      var $544=$543 >>> 16;
      var $545=$544 & 4;
      $K=$545;
      var $546=$K;
      var $547=$N;
      var $548=((($547)+($546))|0);
      $N=$548;
      var $549=$K;
      var $550=$Y;
      var $551=$550 << $549;
      $Y=$551;
      var $552=((($551)-(16384))|0);
      var $553=$552 >>> 16;
      var $554=$553 & 2;
      $K=$554;
      var $555=$N;
      var $556=((($555)+($554))|0);
      $N=$556;
      var $557=$N;
      var $558=(((14)-($557))|0);
      var $559=$K;
      var $560=$Y;
      var $561=$560 << $559;
      $Y=$561;
      var $562=$561 >>> 15;
      var $563=((($558)+($562))|0);
      $K=$563;
      var $564=$K;
      var $565=$564 << 1;
      var $566=$qsize;
      var $567=$K;
      var $568=((($567)+(7))|0);
      var $569=$566 >>> (($568)>>>(0));
      var $570=$569 & 1;
      var $571=((($565)+($570))|0);
      $I7=$571;
      label = 94; break;
    case 94: 
      label = 95; break;
    case 95: 
      var $574=$I7;
      var $575=$1;
      var $576=(($575+304)|0);
      var $577=(($576+($574<<2))|0);
      $H6=$577;
      var $578=$I7;
      var $579=$TP5;
      var $580=(($579+28)|0);
      HEAP32[(($580)>>2)]=$578;
      var $581=$TP5;
      var $582=(($581+16)|0);
      var $583=(($582+4)|0);
      HEAP32[(($583)>>2)]=0;
      var $584=$TP5;
      var $585=(($584+16)|0);
      var $586=(($585)|0);
      HEAP32[(($586)>>2)]=0;
      var $587=$1;
      var $588=(($587+4)|0);
      var $589=HEAP32[(($588)>>2)];
      var $590=$I7;
      var $591=1 << $590;
      var $592=$589 & $591;
      var $593=(($592)|(0))!=0;
      if ($593) { label = 97; break; } else { label = 96; break; }
    case 96: 
      var $595=$I7;
      var $596=1 << $595;
      var $597=$1;
      var $598=(($597+4)|0);
      var $599=HEAP32[(($598)>>2)];
      var $600=$599 | $596;
      HEAP32[(($598)>>2)]=$600;
      var $601=$TP5;
      var $602=$H6;
      HEAP32[(($602)>>2)]=$601;
      var $603=$H6;
      var $604=$603;
      var $605=$TP5;
      var $606=(($605+24)|0);
      HEAP32[(($606)>>2)]=$604;
      var $607=$TP5;
      var $608=$TP5;
      var $609=(($608+12)|0);
      HEAP32[(($609)>>2)]=$607;
      var $610=$TP5;
      var $611=(($610+8)|0);
      HEAP32[(($611)>>2)]=$607;
      label = 115; break;
    case 97: 
      var $613=$H6;
      var $614=HEAP32[(($613)>>2)];
      $T=$614;
      var $615=$qsize;
      var $616=$I7;
      var $617=(($616)|(0))==31;
      if ($617) { label = 98; break; } else { label = 99; break; }
    case 98: 
      var $626 = 0;label = 100; break;
    case 99: 
      var $620=$I7;
      var $621=$620 >>> 1;
      var $622=((($621)+(8))|0);
      var $623=((($622)-(2))|0);
      var $624=(((31)-($623))|0);
      var $626 = $624;label = 100; break;
    case 100: 
      var $626;
      var $627=$615 << $626;
      $K8=$627;
      label = 101; break;
    case 101: 
      var $629=$T;
      var $630=(($629+4)|0);
      var $631=HEAP32[(($630)>>2)];
      var $632=$631 & -8;
      var $633=$qsize;
      var $634=(($632)|(0))!=(($633)|(0));
      if ($634) { label = 102; break; } else { label = 108; break; }
    case 102: 
      var $636=$K8;
      var $637=$636 >>> 31;
      var $638=$637 & 1;
      var $639=$T;
      var $640=(($639+16)|0);
      var $641=(($640+($638<<2))|0);
      $C=$641;
      var $642=$K8;
      var $643=$642 << 1;
      $K8=$643;
      var $644=$C;
      var $645=HEAP32[(($644)>>2)];
      var $646=(($645)|(0))!=0;
      if ($646) { label = 103; break; } else { label = 104; break; }
    case 103: 
      var $648=$C;
      var $649=HEAP32[(($648)>>2)];
      $T=$649;
      label = 107; break;
    case 104: 
      var $651=$C;
      var $652=$651;
      var $653=$1;
      var $654=(($653+16)|0);
      var $655=HEAP32[(($654)>>2)];
      var $656=(($652)>>>(0)) >= (($655)>>>(0));
      var $657=(($656)&(1));
      var $658=($657);
      var $659=(($658)|(0))!=0;
      if ($659) { label = 105; break; } else { label = 106; break; }
    case 105: 
      var $661=$TP5;
      var $662=$C;
      HEAP32[(($662)>>2)]=$661;
      var $663=$T;
      var $664=$TP5;
      var $665=(($664+24)|0);
      HEAP32[(($665)>>2)]=$663;
      var $666=$TP5;
      var $667=$TP5;
      var $668=(($667+12)|0);
      HEAP32[(($668)>>2)]=$666;
      var $669=$TP5;
      var $670=(($669+8)|0);
      HEAP32[(($670)>>2)]=$666;
      label = 114; break;
    case 106: 
      _abort();
      throw "Reached an unreachable!";
    case 107: 
      label = 113; break;
    case 108: 
      var $674=$T;
      var $675=(($674+8)|0);
      var $676=HEAP32[(($675)>>2)];
      $F9=$676;
      var $677=$T;
      var $678=$677;
      var $679=$1;
      var $680=(($679+16)|0);
      var $681=HEAP32[(($680)>>2)];
      var $682=(($678)>>>(0)) >= (($681)>>>(0));
      if ($682) { label = 109; break; } else { var $691 = 0;label = 110; break; }
    case 109: 
      var $684=$F9;
      var $685=$684;
      var $686=$1;
      var $687=(($686+16)|0);
      var $688=HEAP32[(($687)>>2)];
      var $689=(($685)>>>(0)) >= (($688)>>>(0));
      var $691 = $689;label = 110; break;
    case 110: 
      var $691;
      var $692=(($691)&(1));
      var $693=($692);
      var $694=(($693)|(0))!=0;
      if ($694) { label = 111; break; } else { label = 112; break; }
    case 111: 
      var $696=$TP5;
      var $697=$F9;
      var $698=(($697+12)|0);
      HEAP32[(($698)>>2)]=$696;
      var $699=$T;
      var $700=(($699+8)|0);
      HEAP32[(($700)>>2)]=$696;
      var $701=$F9;
      var $702=$TP5;
      var $703=(($702+8)|0);
      HEAP32[(($703)>>2)]=$701;
      var $704=$T;
      var $705=$TP5;
      var $706=(($705+12)|0);
      HEAP32[(($706)>>2)]=$704;
      var $707=$TP5;
      var $708=(($707+24)|0);
      HEAP32[(($708)>>2)]=0;
      label = 114; break;
    case 112: 
      _abort();
      throw "Reached an unreachable!";
    case 113: 
      label = 101; break;
    case 114: 
      label = 115; break;
    case 115: 
      label = 116; break;
    case 116: 
      label = 117; break;
    case 117: 
      label = 118; break;
    case 118: 
      var $716=$p;
      var $717=$716;
      var $718=(($717+8)|0);
      return $718;
    default: assert(0, "bad label: " + label);
  }
}
function __ZdlPv($ptr) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      $1=$ptr;
      var $2=$1;
      var $3=(($2)|(0))!=0;
      if ($3) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $5=$1;
      _free($5);
      label = 3; break;
    case 3: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
function _add_segment($m, $tbase, $tsize, $mmapped) {
  var label = 0;
  label = 1; 
  while(1) switch(label) {
    case 1: 
      var $1;
      var $2;
      var $3;
      var $4;
      var $old_top;
      var $oldsp;
      var $old_end;
      var $ssize;
      var $rawsp;
      var $offset;
      var $asp;
      var $csp;
      var $sp;
      var $ss;
      var $tnext;
      var $p;
      var $nfences;
      var $nextp;
      var $q;
      var $psize;
      var $tn;
      var $I;
      var $B;
      var $F;
      var $TP;
      var $H;
      var $I1;
      var $X;
      var $Y;
      var $N;
      var $K;
      var $T;
      var $K2;
      var $C;
      var $F3;
      $1=$m;
      $2=$tbase;
      $3=$tsize;
      $4=$mmapped;
      var $5=$1;
      var $6=(($5+24)|0);
      var $7=HEAP32[(($6)>>2)];
      var $8=$7;
      $old_top=$8;
      var $9=$1;
      var $10=$old_top;
      var $11=_segment_holding($9, $10);
      $oldsp=$11;
      var $12=$oldsp;
      var $13=(($12)|0);
      var $14=HEAP32[(($13)>>2)];
      var $15=$oldsp;
      var $16=(($15+4)|0);
      var $17=HEAP32[(($16)>>2)];
      var $18=(($14+$17)|0);
      $old_end=$18;
      $ssize=24;
      var $19=$old_end;
      var $20=$ssize;
      var $21=((($20)+(16))|0);
      var $22=((($21)+(7))|0);
      var $23=(((-$22))|0);
      var $24=(($19+$23)|0);
      $rawsp=$24;
      var $25=$rawsp;
      var $26=(($25+8)|0);
      var $27=$26;
      var $28=$27 & 7;
      var $29=(($28)|(0))==0;
      if ($29) { label = 2; break; } else { label = 3; break; }
    case 2: 
      var $39 = 0;label = 4; break;
    case 3: 
      var $32=$rawsp;
      var $33=(($32+8)|0);
      var $34=$33;
      var $35=$34 & 7;
      var $36=(((8)-($35))|0);
      var $37=$36 & 7;
      var $39 = $37;label = 4; break;
    case 4: 
      var $39;
      $offset=$39;
      var $40=$rawsp;
      var $41=$offset;
      var $42=(($40+$41)|0);
      $asp=$42;
      var $43=$asp;
      var $44=$old_top;
      var $45=(($44+16)|0);
      var $46=(($43)>>>(0)) < (($45)>>>(0));
      if ($46) { label = 5; break; } else { label = 6; break; }
    case 5: 
      var $48=$old_top;
      var $52 = $48;label = 7; break;
    case 6: 
      var $50=$asp;
      var $52 = $50;label = 7; break;
    case 7: 
      var $52;
      $csp=$52;
      var $53=$csp;
      var $54=$53;
      $sp=$54;
      var $55=$sp;
      var $56=$55;
      var $57=(($56+8)|0);
      var $58=$57;
      $ss=$58;
      var $59=$sp;
      var $60=$59;
      var $61=$ssize;
      var $62=(($60+$61)|0);
      var $63=$62;
      $tnext=$63;
      var $64=$tnext;
      $p=$64;
      $nfences=0;
      var $65=$1;
      var $66=$2;
      var $67=$66;
      var $68=$3;
      var $69=((($68)-(40))|0);
      _init_top($65, $67, $69);
      var $70=$ssize;
      var $71=$70 | 1;
      var $72=$71 | 2;
      var $73=$sp;
      var $74=(($73+4)|0);
      HEAP32[(($74)>>2)]=$72;
      var $75=$ss;
      var $76=$1;
      var $77=(($76+448)|0);
      var $78=$75;
      var $79=$77;
      assert(16 % 1 === 0);HEAP32[(($78)>>2)]=HEAP32[(($79)>>2)];HEAP32[((($78)+(4))>>2)]=HEAP32[((($79)+(4))>>2)];HEAP32[((($78)+(8))>>2)]=HEAP32[((($79)+(8))>>2)];HEAP32[((($78)+(12))>>2)]=HEAP32[((($79)+(12))>>2)];
      var $80=$2;
      var $81=$1;
      var $82=(($81+448)|0);
      var $83=(($82)|0);
      HEAP32[(($83)>>2)]=$80;
      var $84=$3;
      var $85=$1;
      var $86=(($85+448)|0);
      var $87=(($86+4)|0);
      HEAP32[(($87)>>2)]=$84;
      var $88=$4;
      var $89=$1;
      var $90=(($89+448)|0);
      var $91=(($90+12)|0);
      HEAP32[(($91)>>2)]=$88;
      var $92=$ss;
      var $93=$1;
      var $94=(($93+448)|0);
      var $95=(($94+8)|0);
      HEAP32[(($95)>>2)]=$92;
      label = 8; break;
    case 8: 
      var $97=$p;
      var $98=$97;
      var $99=(($98+4)|0);
      var $100=$99;
      $nextp=$100;
      var $101=$p;
      var $102=(($101+4)|0);
      HEAP32[(($102)>>2)]=7;
      var $103=$nfences;
      var $104=((($103)+(1))|0);
      $nfences=$104;
      var $105=$nextp;
      var $106=(($105+4)|0);
      var $107=$106;
      var $108=$old_end;
      var $109=(($107)>>>(0)) < (($108)>>>(0));
      if ($109) { label = 9; break; } else { label = 10; break; }
    case 9: 
      var $111=$nextp;
      $p=$111;
      label = 11; break;
    case 10: 
      label = 12; break;
    case 11: 
      label = 8; break;
    case 12: 
      var $115=$csp;
      var $116=$old_top;
      var $117=(($115)|(0))!=(($116)|(0));
      if ($117) { label = 13; break; } else { label = 49; break; }
    case 13: 
      var $119=$old_top;
      var $120=$119;
      $q=$120;
      var $121=$csp;
      var $122=$old_top;
      var $123=$121;
      var $124=$122;
      var $125=((($123)-($124))|0);
      $psize=$125;
      var $126=$q;
      var $127=$126;
      var $128=$psize;
      var $129=(($127+$128)|0);
      var $130=$129;
      $tn=$130;
      var $131=$tn;
      var $132=(($131+4)|0);
      var $133=HEAP32[(($132)>>2)];
      var $134=$133 & -2;
      HEAP32[(($132)>>2)]=$134;
      var $135=$psize;
      var $136=$135 | 1;
      var $137=$q;
      var $138=(($137+4)|0);
      HEAP32[(($138)>>2)]=$136;
      var $139=$psize;
      var $140=$q;
      var $141=$140;
      var $142=$psize;
      var $143=(($141+$142)|0);
      var $144=$143;
      var $145=(($144)|0);
      HEAP32[(($145)>>2)]=$139;
      var $146=$psize;
      var $147=$146 >>> 3;
      var $148=(($147)>>>(0)) < 32;
      if ($148) { label = 14; break; } else { label = 21; break; }
    case 14: 
      var $150=$psize;
      var $151=$150 >>> 3;
      $I=$151;
      var $152=$I;
      var $153=$152 << 1;
      var $154=$1;
      var $155=(($154+40)|0);
      var $156=(($155+($153<<2))|0);
      var $157=$156;
      var $158=$157;
      $B=$158;
      var $159=$B;
      $F=$159;
      var $160=$1;
      var $161=(($160)|0);
      var $162=HEAP32[(($161)>>2)];
      var $163=$I;
      var $164=1 << $163;
      var $165=$162 & $164;
      var $166=(($165)|(0))!=0;
      if ($166) { label = 16; break; } else { label = 15; break; }
    case 15: 
      var $168=$I;
      var $169=1 << $168;
      var $170=$1;
      var $171=(($170)|0);
      var $172=HEAP32[(($171)>>2)];
      var $173=$172 | $169;
      HEAP32[(($171)>>2)]=$173;
      label = 20; break;
    case 16: 
      var $175=$B;
      var $176=(($175+8)|0);
      var $177=HEAP32[(($176)>>2)];
      var $178=$177;
      var $179=$1;
      var $180=(($179+16)|0);
      var $181=HEAP32[(($180)>>2)];
      var $182=(($178)>>>(0)) >= (($181)>>>(0));
      var $183=(($182)&(1));
      var $184=($183);
      var $185=(($184)|(0))!=0;
      if ($185) { label = 17; break; } else { label = 18; break; }
    case 17: 
      var $187=$B;
      var $188=(($187+8)|0);
      var $189=HEAP32[(($188)>>2)];
      $F=$189;
      label = 19; break;
    case 18: 
      _abort();
      throw "Reached an unreachable!";
    case 19: 
      label = 20; break;
    case 20: 
      var $193=$q;
      var $194=$B;
      var $195=(($194+8)|0);
      HEAP32[(($195)>>2)]=$193;
      var $196=$q;
      var $197=$F;
      var $198=(($197+12)|0);
      HEAP32[(($198)>>2)]=$196;
      var $199=$F;
      var $200=$q;
      var $201=(($200+8)|0);
      HEAP32[(($201)>>2)]=$199;
      var $202=$B;
      var $203=$q;
      var $204=(($203+12)|0);
      HEAP32[(($204)>>2)]=$202;
      label = 48; break;
    case 21: 
      var $206=$q;
      var $207=$206;
      $TP=$207;
      var $208=$psize;
      var $209=$208 >>> 8;
      $X=$209;
      var $210=$X;
      var $211=(($210)|(0))==0;
      if ($211) { label = 22; break; } else { label = 23; break; }
    case 22: 
      $I1=0;
      label = 27; break;
    case 23: 
      var $214=$X;
      var $215=(($214)>>>(0)) > 65535;
      if ($215) { label = 24; break; } else { label = 25; break; }
    case 24: 
      $I1=31;
      label = 26; break;
    case 25: 
      var $218=$X;
      $Y=$218;
      var $219=$Y;
      var $220=((($219)-(256))|0);
      var $221=$220 >>> 16;
      var $222=$221 & 8;
      $N=$222;
      var $223=$N;
      var $224=$Y;
      var $225=$224 << $223;
      $Y=$225;
      var $226=((($225)-(4096))|0);
      var $227=$226 >>> 16;
      var $228=$227 & 4;
      $K=$228;
      var $229=$K;
      var $230=$N;
      var $231=((($230)+($229))|0);
      $N=$231;
      var $232=$K;
      var $233=$Y;
      var $234=$233 << $232;
      $Y=$234;
      var $235=((($234)-(16384))|0);
      var $236=$235 >>> 16;
      var $237=$236 & 2;
      $K=$237;
      var $238=$N;
      var $239=((($238)+($237))|0);
      $N=$239;
      var $240=$N;
      var $241=(((14)-($240))|0);
      var $242=$K;
      var $243=$Y;
      var $244=$243 << $242;
      $Y=$244;
      var $245=$244 >>> 15;
      var $246=((($241)+($245))|0);
      $K=$246;
      var $247=$K;
      var $248=$247 << 1;
      var $249=$psize;
      var $250=$K;
      var $251=((($250)+(7))|0);
      var $252=$249 >>> (($251)>>>(0));
      var $253=$252 & 1;
      var $254=((($248)+($253))|0);
      $I1=$254;
      label = 26; break;
    case 26: 
      label = 27; break;
    case 27: 
      var $257=$I1;
      var $258=$1;
      var $259=(($258+304)|0);
      var $260=(($259+($257<<2))|0);
      $H=$260;
      var $261=$I1;
      var $262=$TP;
      var $263=(($262+28)|0);
      HEAP32[(($263)>>2)]=$261;
      var $264=$TP;
      var $265=(($264+16)|0);
      var $266=(($265+4)|0);
      HEAP32[(($266)>>2)]=0;
      var $267=$TP;
      var $268=(($267+16)|0);
      var $269=(($268)|0);
      HEAP32[(($269)>>2)]=0;
      var $270=$1;
      var $271=(($270+4)|0);
      var $272=HEAP32[(($271)>>2)];
      var $273=$I1;
      var $274=1 << $273;
      var $275=$272 & $274;
      var $276=(($275)|(0))!=0;
      if ($276) { label = 29; break; } else { label = 28; break; }
    case 28: 
      var $278=$I1;
      var $279=1 << $278;
      var $280=$1;
      var $281=(($280+4)|0);
      var $282=HEAP32[(($281)>>2)];
      var $283=$282 | $279;
      HEAP32[(($281)>>2)]=$283;
      var $284=$TP;
      var $285=$H;
      HEAP32[(($285)>>2)]=$284;
      var $286=$H;
      var $287=$286;
      var $288=$TP;
      var $289=(($288+24)|0);
      HEAP32[(($289)>>2)]=$287;
      var $290=$TP;
      var $291=$TP;
      var $292=(($291+12)|0);
      HEAP32[(($292)>>2)]=$290;
      var $293=$TP;
      var $294=(($293+8)|0);
      HEAP32[(($294)>>2)]=$290;
      label = 47; break;
    case 29: 
      var $296=$H;
      var $297=HEAP32[(($296)>>2)];
      $T=$297;
      var $298=$psize;
      var $299=$I1;
      var $300=(($299)|(0))==31;
      if ($300) { label = 30; break; } else { label = 31; break; }
    case 30: 
      var $309 = 0;label = 32; break;
    case 31: 
      var $303=$I1;
      var $304=$303 >>> 1;
      var $305=((($304)+(8))|0);
      var $306=((($305)-(2))|0);
      var $307=(((31)-($306))|0);
      var $309 = $307;label = 32; break;
    case 32: 
      var $309;
      var $310=$298 << $309;
      $K2=$310;
      label = 33; break;
    case 33: 
      var $312=$T;
      var $313=(($312+4)|0);
      var $314=HEAP32[(($313)>>2)];
      var $315=$314 & -8;
      var $316=$psize;
      var $317=(($315)|(0))!=(($316)|(0));
      if ($317) { label = 34; break; } else { label = 40; break; }
    case 34: 
      var $319=$K2;
      var $320=$319 >>> 31;
      var $321=$320 & 1;
      var $322=$T;
      var $323=(($322+16)|0);
      var $324=(($323+($321<<2))|0);
      $C=$324;
      var $325=$K2;
      var $326=$325 << 1;
      $K2=$326;
      var $327=$C;
      var $328=HEAP32[(($327)>>2)];
      var $329=(($328)|(0))!=0;
      if ($329) { label = 35; break; } else { label = 36; break; }
    case 35: 
      var $331=$C;
      var $332=HEAP32[(($331)>>2)];
      $T=$332;
      label = 39; break;
    case 36: 
      var $334=$C;
      var $335=$334;
      var $336=$1;
      var $337=(($336+16)|0);
      var $338=HEAP32[(($337)>>2)];
      var $339=(($335)>>>(0)) >= (($338)>>>(0));
      var $340=(($339)&(1));
      var $341=($340);
      var $342=(($341)|(0))!=0;
      if ($342) { label = 37; break; } else { label = 38; break; }
    case 37: 
      var $344=$TP;
      var $345=$C;
      HEAP32[(($345)>>2)]=$344;
      var $346=$T;
      var $347=$TP;
      var $348=(($347+24)|0);
      HEAP32[(($348)>>2)]=$346;
      var $349=$TP;
      var $350=$TP;
      var $351=(($350+12)|0);
      HEAP32[(($351)>>2)]=$349;
      var $352=$TP;
      var $353=(($352+8)|0);
      HEAP32[(($353)>>2)]=$349;
      label = 46; break;
    case 38: 
      _abort();
      throw "Reached an unreachable!";
    case 39: 
      label = 45; break;
    case 40: 
      var $357=$T;
      var $358=(($357+8)|0);
      var $359=HEAP32[(($358)>>2)];
      $F3=$359;
      var $360=$T;
      var $361=$360;
      var $362=$1;
      var $363=(($362+16)|0);
      var $364=HEAP32[(($363)>>2)];
      var $365=(($361)>>>(0)) >= (($364)>>>(0));
      if ($365) { label = 41; break; } else { var $374 = 0;label = 42; break; }
    case 41: 
      var $367=$F3;
      var $368=$367;
      var $369=$1;
      var $370=(($369+16)|0);
      var $371=HEAP32[(($370)>>2)];
      var $372=(($368)>>>(0)) >= (($371)>>>(0));
      var $374 = $372;label = 42; break;
    case 42: 
      var $374;
      var $375=(($374)&(1));
      var $376=($375);
      var $377=(($376)|(0))!=0;
      if ($377) { label = 43; break; } else { label = 44; break; }
    case 43: 
      var $379=$TP;
      var $380=$F3;
      var $381=(($380+12)|0);
      HEAP32[(($381)>>2)]=$379;
      var $382=$T;
      var $383=(($382+8)|0);
      HEAP32[(($383)>>2)]=$379;
      var $384=$F3;
      var $385=$TP;
      var $386=(($385+8)|0);
      HEAP32[(($386)>>2)]=$384;
      var $387=$T;
      var $388=$TP;
      var $389=(($388+12)|0);
      HEAP32[(($389)>>2)]=$387;
      var $390=$TP;
      var $391=(($390+24)|0);
      HEAP32[(($391)>>2)]=0;
      label = 46; break;
    case 44: 
      _abort();
      throw "Reached an unreachable!";
    case 45: 
      label = 33; break;
    case 46: 
      label = 47; break;
    case 47: 
      label = 48; break;
    case 48: 
      label = 49; break;
    case 49: 
      return;
    default: assert(0, "bad label: " + label);
  }
}
// EMSCRIPTEN_END_FUNCS
function _i64Add(a, b, c, d) {
    /*
      x = a + b*2^32
      y = c + d*2^32
      result = l + h*2^32
    */
    a = a|0; b = b|0; c = c|0; d = d|0;
    var l = 0, h = 0;
    l = (a + c)>>>0;
    h = (b + d + (((l>>>0) < (a>>>0))|0))>>>0; // Add carry from low word to high word on overflow.
    return tempRet0 = h,l|0;
  }
function _i64Subtract(a, b, c, d) {
    a = a|0; b = b|0; c = c|0; d = d|0;
    var l = 0, h = 0;
    l = (a - c)>>>0;
    h = (b - d)>>>0;
    h = (b - d - (((c>>>0) > (a>>>0))|0))>>>0; // Borrow one from high word to low word on underflow.
    return tempRet0 = h,l|0;
  }
function _bitshift64Shl(low, high, bits) {
    low = low|0; high = high|0; bits = bits|0;
    var ander = 0;
    if ((bits|0) < 32) {
      ander = ((1 << bits) - 1)|0;
      tempRet0 = (high << bits) | ((low&(ander << (32 - bits))) >>> (32 - bits));
      return low << bits;
    }
    tempRet0 = low << (bits - 32);
    return 0;
  }
function _bitshift64Lshr(low, high, bits) {
    low = low|0; high = high|0; bits = bits|0;
    var ander = 0;
    if ((bits|0) < 32) {
      ander = ((1 << bits) - 1)|0;
      tempRet0 = high >>> bits;
      return (low >>> bits) | ((high&ander) << (32 - bits));
    }
    tempRet0 = 0;
    return (high >>> (bits - 32))|0;
  }
function _bitshift64Ashr(low, high, bits) {
    low = low|0; high = high|0; bits = bits|0;
    var ander = 0;
    if ((bits|0) < 32) {
      ander = ((1 << bits) - 1)|0;
      tempRet0 = high >> bits;
      return (low >>> bits) | ((high&ander) << (32 - bits));
    }
    tempRet0 = (high|0) < 0 ? -1 : 0;
    return (high >> (bits - 32))|0;
  }
function _llvm_ctlz_i32(x) {
    x = x|0;
    var ret = 0;
    ret = HEAP8[(((ctlz_i8)+(x >>> 24))|0)];
    if ((ret|0) < 8) return ret|0;
    var ret = HEAP8[(((ctlz_i8)+((x >> 16)&0xff))|0)];
    if ((ret|0) < 8) return (ret + 8)|0;
    var ret = HEAP8[(((ctlz_i8)+((x >> 8)&0xff))|0)];
    if ((ret|0) < 8) return (ret + 16)|0;
    return (HEAP8[(((ctlz_i8)+(x&0xff))|0)] + 24)|0;
  }
/* PRE_ASM */ var ctlz_i8 = allocate([8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], "i8", ALLOC_DYNAMIC);
function _llvm_cttz_i32(x) {
    x = x|0;
    var ret = 0;
    ret = HEAP8[(((cttz_i8)+(x & 0xff))|0)];
    if ((ret|0) < 8) return ret|0;
    var ret = HEAP8[(((cttz_i8)+((x >> 8)&0xff))|0)];
    if ((ret|0) < 8) return (ret + 8)|0;
    var ret = HEAP8[(((cttz_i8)+((x >> 16)&0xff))|0)];
    if ((ret|0) < 8) return (ret + 16)|0;
    return (HEAP8[(((cttz_i8)+(x >>> 24))|0)] + 24)|0;
  }
/* PRE_ASM */ var cttz_i8 = allocate([8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0], "i8", ALLOC_DYNAMIC);
// ======== compiled code from system/lib/compiler-rt , see readme therein
function ___muldsi3($a, $b) {
  $a = $a | 0;
  $b = $b | 0;
  var $1 = 0, $2 = 0, $3 = 0, $6 = 0, $8 = 0, $11 = 0, $12 = 0;
  $1 = $a & 65535;
  $2 = $b & 65535;
  $3 = Math.imul($2, $1);
  $6 = $a >>> 16;
  $8 = ($3 >>> 16) + Math.imul($2, $6) | 0;
  $11 = $b >>> 16;
  $12 = Math.imul($11, $1);
  return (tempRet0 = (($8 >>> 16) + Math.imul($11, $6) | 0) + ((($8 & 65535) + $12 | 0) >>> 16) | 0, 0 | ($8 + $12 << 16 | $3 & 65535)) | 0;
}
function ___divdi3($a$0, $a$1, $b$0, $b$1) {
  $a$0 = $a$0 | 0;
  $a$1 = $a$1 | 0;
  $b$0 = $b$0 | 0;
  $b$1 = $b$1 | 0;
  var $1$0 = 0, $1$1 = 0, $2$0 = 0, $2$1 = 0, $4$0 = 0, $4$1 = 0, $6$0 = 0, $7$0 = 0, $7$1 = 0, $8$0 = 0, $10$0 = 0;
  $1$0 = $a$1 >> 31 | (($a$1 | 0) < 0 ? -1 : 0) << 1;
  $1$1 = (($a$1 | 0) < 0 ? -1 : 0) >> 31 | (($a$1 | 0) < 0 ? -1 : 0) << 1;
  $2$0 = $b$1 >> 31 | (($b$1 | 0) < 0 ? -1 : 0) << 1;
  $2$1 = (($b$1 | 0) < 0 ? -1 : 0) >> 31 | (($b$1 | 0) < 0 ? -1 : 0) << 1;
  $4$0 = _i64Subtract($1$0 ^ $a$0, $1$1 ^ $a$1, $1$0, $1$1) | 0;
  $4$1 = tempRet0;
  $6$0 = _i64Subtract($2$0 ^ $b$0, $2$1 ^ $b$1, $2$0, $2$1) | 0;
  $7$0 = $2$0 ^ $1$0;
  $7$1 = $2$1 ^ $1$1;
  $8$0 = ___udivmoddi4($4$0, $4$1, $6$0, tempRet0, 0) | 0;
  $10$0 = _i64Subtract($8$0 ^ $7$0, tempRet0 ^ $7$1, $7$0, $7$1) | 0;
  return (tempRet0 = tempRet0, $10$0) | 0;
}
function ___remdi3($a$0, $a$1, $b$0, $b$1) {
  $a$0 = $a$0 | 0;
  $a$1 = $a$1 | 0;
  $b$0 = $b$0 | 0;
  $b$1 = $b$1 | 0;
  var $rem = 0, $1$0 = 0, $1$1 = 0, $2$0 = 0, $2$1 = 0, $4$0 = 0, $4$1 = 0, $6$0 = 0, $10$0 = 0, $10$1 = 0, __stackBase__ = 0;
  __stackBase__ = STACKTOP;
  STACKTOP = STACKTOP + 8 | 0;
  $rem = __stackBase__ | 0;
  $1$0 = $a$1 >> 31 | (($a$1 | 0) < 0 ? -1 : 0) << 1;
  $1$1 = (($a$1 | 0) < 0 ? -1 : 0) >> 31 | (($a$1 | 0) < 0 ? -1 : 0) << 1;
  $2$0 = $b$1 >> 31 | (($b$1 | 0) < 0 ? -1 : 0) << 1;
  $2$1 = (($b$1 | 0) < 0 ? -1 : 0) >> 31 | (($b$1 | 0) < 0 ? -1 : 0) << 1;
  $4$0 = _i64Subtract($1$0 ^ $a$0, $1$1 ^ $a$1, $1$0, $1$1) | 0;
  $4$1 = tempRet0;
  $6$0 = _i64Subtract($2$0 ^ $b$0, $2$1 ^ $b$1, $2$0, $2$1) | 0;
  ___udivmoddi4($4$0, $4$1, $6$0, tempRet0, $rem);
  $10$0 = _i64Subtract(HEAP32[$rem >> 2] ^ $1$0, HEAP32[$rem + 4 >> 2] ^ $1$1, $1$0, $1$1) | 0;
  $10$1 = tempRet0;
  STACKTOP = __stackBase__;
  return (tempRet0 = $10$1, $10$0) | 0;
}
function ___muldi3($a$0, $a$1, $b$0, $b$1) {
  $a$0 = $a$0 | 0;
  $a$1 = $a$1 | 0;
  $b$0 = $b$0 | 0;
  $b$1 = $b$1 | 0;
  var $x_sroa_0_0_extract_trunc = 0, $y_sroa_0_0_extract_trunc = 0, $1$0 = 0, $1$1 = 0, $2 = 0;
  $x_sroa_0_0_extract_trunc = $a$0;
  $y_sroa_0_0_extract_trunc = $b$0;
  $1$0 = ___muldsi3($x_sroa_0_0_extract_trunc, $y_sroa_0_0_extract_trunc) | 0;
  $1$1 = tempRet0;
  $2 = Math.imul($a$1, $y_sroa_0_0_extract_trunc);
  return (tempRet0 = (Math.imul($b$1, $x_sroa_0_0_extract_trunc) + $2 | 0) + $1$1 | $1$1 & 0, 0 | $1$0 & -1) | 0;
}
function ___udivdi3($a$0, $a$1, $b$0, $b$1) {
  $a$0 = $a$0 | 0;
  $a$1 = $a$1 | 0;
  $b$0 = $b$0 | 0;
  $b$1 = $b$1 | 0;
  var $1$0 = 0;
  $1$0 = ___udivmoddi4($a$0, $a$1, $b$0, $b$1, 0) | 0;
  return (tempRet0 = tempRet0, $1$0) | 0;
}
function ___uremdi3($a$0, $a$1, $b$0, $b$1) {
  $a$0 = $a$0 | 0;
  $a$1 = $a$1 | 0;
  $b$0 = $b$0 | 0;
  $b$1 = $b$1 | 0;
  var $rem = 0, __stackBase__ = 0;
  __stackBase__ = STACKTOP;
  STACKTOP = STACKTOP + 8 | 0;
  $rem = __stackBase__ | 0;
  ___udivmoddi4($a$0, $a$1, $b$0, $b$1, $rem);
  STACKTOP = __stackBase__;
  return (tempRet0 = HEAP32[$rem + 4 >> 2] | 0, HEAP32[$rem >> 2] | 0) | 0;
}
function ___udivmoddi4($a$0, $a$1, $b$0, $b$1, $rem) {
  $a$0 = $a$0 | 0;
  $a$1 = $a$1 | 0;
  $b$0 = $b$0 | 0;
  $b$1 = $b$1 | 0;
  $rem = $rem | 0;
  var $n_sroa_0_0_extract_trunc = 0, $n_sroa_1_4_extract_shift$0 = 0, $n_sroa_1_4_extract_trunc = 0, $d_sroa_0_0_extract_trunc = 0, $d_sroa_1_4_extract_shift$0 = 0, $d_sroa_1_4_extract_trunc = 0, $4 = 0, $17 = 0, $37 = 0, $49 = 0, $51 = 0, $57 = 0, $58 = 0, $66 = 0, $78 = 0, $86 = 0, $88 = 0, $89 = 0, $91 = 0, $92 = 0, $95 = 0, $105 = 0, $117 = 0, $119 = 0, $125 = 0, $126 = 0, $130 = 0, $q_sroa_1_1_ph = 0, $q_sroa_0_1_ph = 0, $r_sroa_1_1_ph = 0, $r_sroa_0_1_ph = 0, $sr_1_ph = 0, $d_sroa_0_0_insert_insert99$0 = 0, $d_sroa_0_0_insert_insert99$1 = 0, $137$0 = 0, $137$1 = 0, $carry_0203 = 0, $sr_1202 = 0, $r_sroa_0_1201 = 0, $r_sroa_1_1200 = 0, $q_sroa_0_1199 = 0, $q_sroa_1_1198 = 0, $147 = 0, $149 = 0, $r_sroa_0_0_insert_insert42$0 = 0, $r_sroa_0_0_insert_insert42$1 = 0, $150$1 = 0, $151$0 = 0, $152 = 0, $154$0 = 0, $r_sroa_0_0_extract_trunc = 0, $r_sroa_1_4_extract_trunc = 0, $155 = 0, $carry_0_lcssa$0 = 0, $carry_0_lcssa$1 = 0, $r_sroa_0_1_lcssa = 0, $r_sroa_1_1_lcssa = 0, $q_sroa_0_1_lcssa = 0, $q_sroa_1_1_lcssa = 0, $q_sroa_0_0_insert_ext75$0 = 0, $q_sroa_0_0_insert_ext75$1 = 0, $q_sroa_0_0_insert_insert77$1 = 0, $_0$0 = 0, $_0$1 = 0;
  $n_sroa_0_0_extract_trunc = $a$0;
  $n_sroa_1_4_extract_shift$0 = $a$1;
  $n_sroa_1_4_extract_trunc = $n_sroa_1_4_extract_shift$0;
  $d_sroa_0_0_extract_trunc = $b$0;
  $d_sroa_1_4_extract_shift$0 = $b$1;
  $d_sroa_1_4_extract_trunc = $d_sroa_1_4_extract_shift$0;
  if (($n_sroa_1_4_extract_trunc | 0) == 0) {
    $4 = ($rem | 0) != 0;
    if (($d_sroa_1_4_extract_trunc | 0) == 0) {
      if ($4) {
        HEAP32[$rem >> 2] = ($n_sroa_0_0_extract_trunc >>> 0) % ($d_sroa_0_0_extract_trunc >>> 0);
        HEAP32[$rem + 4 >> 2] = 0;
      }
      $_0$1 = 0;
      $_0$0 = ($n_sroa_0_0_extract_trunc >>> 0) / ($d_sroa_0_0_extract_trunc >>> 0) >>> 0;
      return (tempRet0 = $_0$1, $_0$0) | 0;
    } else {
      if (!$4) {
        $_0$1 = 0;
        $_0$0 = 0;
        return (tempRet0 = $_0$1, $_0$0) | 0;
      }
      HEAP32[$rem >> 2] = $a$0 & -1;
      HEAP32[$rem + 4 >> 2] = $a$1 & 0;
      $_0$1 = 0;
      $_0$0 = 0;
      return (tempRet0 = $_0$1, $_0$0) | 0;
    }
  }
  $17 = ($d_sroa_1_4_extract_trunc | 0) == 0;
  do {
    if (($d_sroa_0_0_extract_trunc | 0) == 0) {
      if ($17) {
        if (($rem | 0) != 0) {
          HEAP32[$rem >> 2] = ($n_sroa_1_4_extract_trunc >>> 0) % ($d_sroa_0_0_extract_trunc >>> 0);
          HEAP32[$rem + 4 >> 2] = 0;
        }
        $_0$1 = 0;
        $_0$0 = ($n_sroa_1_4_extract_trunc >>> 0) / ($d_sroa_0_0_extract_trunc >>> 0) >>> 0;
        return (tempRet0 = $_0$1, $_0$0) | 0;
      }
      if (($n_sroa_0_0_extract_trunc | 0) == 0) {
        if (($rem | 0) != 0) {
          HEAP32[$rem >> 2] = 0;
          HEAP32[$rem + 4 >> 2] = ($n_sroa_1_4_extract_trunc >>> 0) % ($d_sroa_1_4_extract_trunc >>> 0);
        }
        $_0$1 = 0;
        $_0$0 = ($n_sroa_1_4_extract_trunc >>> 0) / ($d_sroa_1_4_extract_trunc >>> 0) >>> 0;
        return (tempRet0 = $_0$1, $_0$0) | 0;
      }
      $37 = $d_sroa_1_4_extract_trunc - 1 | 0;
      if (($37 & $d_sroa_1_4_extract_trunc | 0) == 0) {
        if (($rem | 0) != 0) {
          HEAP32[$rem >> 2] = 0 | $a$0 & -1;
          HEAP32[$rem + 4 >> 2] = $37 & $n_sroa_1_4_extract_trunc | $a$1 & 0;
        }
        $_0$1 = 0;
        $_0$0 = $n_sroa_1_4_extract_trunc >>> ((_llvm_cttz_i32($d_sroa_1_4_extract_trunc | 0) | 0) >>> 0);
        return (tempRet0 = $_0$1, $_0$0) | 0;
      }
      $49 = _llvm_ctlz_i32($d_sroa_1_4_extract_trunc | 0) | 0;
      $51 = $49 - (_llvm_ctlz_i32($n_sroa_1_4_extract_trunc | 0) | 0) | 0;
      if ($51 >>> 0 <= 30) {
        $57 = $51 + 1 | 0;
        $58 = 31 - $51 | 0;
        $sr_1_ph = $57;
        $r_sroa_0_1_ph = $n_sroa_1_4_extract_trunc << $58 | $n_sroa_0_0_extract_trunc >>> ($57 >>> 0);
        $r_sroa_1_1_ph = $n_sroa_1_4_extract_trunc >>> ($57 >>> 0);
        $q_sroa_0_1_ph = 0;
        $q_sroa_1_1_ph = $n_sroa_0_0_extract_trunc << $58;
        break;
      }
      if (($rem | 0) == 0) {
        $_0$1 = 0;
        $_0$0 = 0;
        return (tempRet0 = $_0$1, $_0$0) | 0;
      }
      HEAP32[$rem >> 2] = 0 | $a$0 & -1;
      HEAP32[$rem + 4 >> 2] = $n_sroa_1_4_extract_shift$0 | $a$1 & 0;
      $_0$1 = 0;
      $_0$0 = 0;
      return (tempRet0 = $_0$1, $_0$0) | 0;
    } else {
      if (!$17) {
        $117 = _llvm_ctlz_i32($d_sroa_1_4_extract_trunc | 0) | 0;
        $119 = $117 - (_llvm_ctlz_i32($n_sroa_1_4_extract_trunc | 0) | 0) | 0;
        if ($119 >>> 0 <= 31) {
          $125 = $119 + 1 | 0;
          $126 = 31 - $119 | 0;
          $130 = $119 - 31 >> 31;
          $sr_1_ph = $125;
          $r_sroa_0_1_ph = $n_sroa_0_0_extract_trunc >>> ($125 >>> 0) & $130 | $n_sroa_1_4_extract_trunc << $126;
          $r_sroa_1_1_ph = $n_sroa_1_4_extract_trunc >>> ($125 >>> 0) & $130;
          $q_sroa_0_1_ph = 0;
          $q_sroa_1_1_ph = $n_sroa_0_0_extract_trunc << $126;
          break;
        }
        if (($rem | 0) == 0) {
          $_0$1 = 0;
          $_0$0 = 0;
          return (tempRet0 = $_0$1, $_0$0) | 0;
        }
        HEAP32[$rem >> 2] = 0 | $a$0 & -1;
        HEAP32[$rem + 4 >> 2] = $n_sroa_1_4_extract_shift$0 | $a$1 & 0;
        $_0$1 = 0;
        $_0$0 = 0;
        return (tempRet0 = $_0$1, $_0$0) | 0;
      }
      $66 = $d_sroa_0_0_extract_trunc - 1 | 0;
      if (($66 & $d_sroa_0_0_extract_trunc | 0) != 0) {
        $86 = (_llvm_ctlz_i32($d_sroa_0_0_extract_trunc | 0) | 0) + 33 | 0;
        $88 = $86 - (_llvm_ctlz_i32($n_sroa_1_4_extract_trunc | 0) | 0) | 0;
        $89 = 64 - $88 | 0;
        $91 = 32 - $88 | 0;
        $92 = $91 >> 31;
        $95 = $88 - 32 | 0;
        $105 = $95 >> 31;
        $sr_1_ph = $88;
        $r_sroa_0_1_ph = $91 - 1 >> 31 & $n_sroa_1_4_extract_trunc >>> ($95 >>> 0) | ($n_sroa_1_4_extract_trunc << $91 | $n_sroa_0_0_extract_trunc >>> ($88 >>> 0)) & $105;
        $r_sroa_1_1_ph = $105 & $n_sroa_1_4_extract_trunc >>> ($88 >>> 0);
        $q_sroa_0_1_ph = $n_sroa_0_0_extract_trunc << $89 & $92;
        $q_sroa_1_1_ph = ($n_sroa_1_4_extract_trunc << $89 | $n_sroa_0_0_extract_trunc >>> ($95 >>> 0)) & $92 | $n_sroa_0_0_extract_trunc << $91 & $88 - 33 >> 31;
        break;
      }
      if (($rem | 0) != 0) {
        HEAP32[$rem >> 2] = $66 & $n_sroa_0_0_extract_trunc;
        HEAP32[$rem + 4 >> 2] = 0;
      }
      if (($d_sroa_0_0_extract_trunc | 0) == 1) {
        $_0$1 = $n_sroa_1_4_extract_shift$0 | $a$1 & 0;
        $_0$0 = 0 | $a$0 & -1;
        return (tempRet0 = $_0$1, $_0$0) | 0;
      } else {
        $78 = _llvm_cttz_i32($d_sroa_0_0_extract_trunc | 0) | 0;
        $_0$1 = 0 | $n_sroa_1_4_extract_trunc >>> ($78 >>> 0);
        $_0$0 = $n_sroa_1_4_extract_trunc << 32 - $78 | $n_sroa_0_0_extract_trunc >>> ($78 >>> 0) | 0;
        return (tempRet0 = $_0$1, $_0$0) | 0;
      }
    }
  } while (0);
  if (($sr_1_ph | 0) == 0) {
    $q_sroa_1_1_lcssa = $q_sroa_1_1_ph;
    $q_sroa_0_1_lcssa = $q_sroa_0_1_ph;
    $r_sroa_1_1_lcssa = $r_sroa_1_1_ph;
    $r_sroa_0_1_lcssa = $r_sroa_0_1_ph;
    $carry_0_lcssa$1 = 0;
    $carry_0_lcssa$0 = 0;
  } else {
    $d_sroa_0_0_insert_insert99$0 = 0 | $b$0 & -1;
    $d_sroa_0_0_insert_insert99$1 = $d_sroa_1_4_extract_shift$0 | $b$1 & 0;
    $137$0 = _i64Add($d_sroa_0_0_insert_insert99$0, $d_sroa_0_0_insert_insert99$1, -1, -1) | 0;
    $137$1 = tempRet0;
    $q_sroa_1_1198 = $q_sroa_1_1_ph;
    $q_sroa_0_1199 = $q_sroa_0_1_ph;
    $r_sroa_1_1200 = $r_sroa_1_1_ph;
    $r_sroa_0_1201 = $r_sroa_0_1_ph;
    $sr_1202 = $sr_1_ph;
    $carry_0203 = 0;
    while (1) {
      $147 = $q_sroa_0_1199 >>> 31 | $q_sroa_1_1198 << 1;
      $149 = $carry_0203 | $q_sroa_0_1199 << 1;
      $r_sroa_0_0_insert_insert42$0 = 0 | ($r_sroa_0_1201 << 1 | $q_sroa_1_1198 >>> 31);
      $r_sroa_0_0_insert_insert42$1 = $r_sroa_0_1201 >>> 31 | $r_sroa_1_1200 << 1 | 0;
      _i64Subtract($137$0, $137$1, $r_sroa_0_0_insert_insert42$0, $r_sroa_0_0_insert_insert42$1);
      $150$1 = tempRet0;
      $151$0 = $150$1 >> 31 | (($150$1 | 0) < 0 ? -1 : 0) << 1;
      $152 = $151$0 & 1;
      $154$0 = _i64Subtract($r_sroa_0_0_insert_insert42$0, $r_sroa_0_0_insert_insert42$1, $151$0 & $d_sroa_0_0_insert_insert99$0, ((($150$1 | 0) < 0 ? -1 : 0) >> 31 | (($150$1 | 0) < 0 ? -1 : 0) << 1) & $d_sroa_0_0_insert_insert99$1) | 0;
      $r_sroa_0_0_extract_trunc = $154$0;
      $r_sroa_1_4_extract_trunc = tempRet0;
      $155 = $sr_1202 - 1 | 0;
      if (($155 | 0) == 0) {
        break;
      } else {
        $q_sroa_1_1198 = $147;
        $q_sroa_0_1199 = $149;
        $r_sroa_1_1200 = $r_sroa_1_4_extract_trunc;
        $r_sroa_0_1201 = $r_sroa_0_0_extract_trunc;
        $sr_1202 = $155;
        $carry_0203 = $152;
      }
    }
    $q_sroa_1_1_lcssa = $147;
    $q_sroa_0_1_lcssa = $149;
    $r_sroa_1_1_lcssa = $r_sroa_1_4_extract_trunc;
    $r_sroa_0_1_lcssa = $r_sroa_0_0_extract_trunc;
    $carry_0_lcssa$1 = 0;
    $carry_0_lcssa$0 = $152;
  }
  $q_sroa_0_0_insert_ext75$0 = $q_sroa_0_1_lcssa;
  $q_sroa_0_0_insert_ext75$1 = 0;
  $q_sroa_0_0_insert_insert77$1 = $q_sroa_1_1_lcssa | $q_sroa_0_0_insert_ext75$1;
  if (($rem | 0) != 0) {
    HEAP32[$rem >> 2] = 0 | $r_sroa_0_1_lcssa;
    HEAP32[$rem + 4 >> 2] = $r_sroa_1_1_lcssa | 0;
  }
  $_0$1 = (0 | $q_sroa_0_0_insert_ext75$0) >>> 31 | $q_sroa_0_0_insert_insert77$1 << 1 | ($q_sroa_0_0_insert_ext75$1 << 1 | $q_sroa_0_0_insert_ext75$0 >>> 31) & 0 | $carry_0_lcssa$1;
  $_0$0 = ($q_sroa_0_0_insert_ext75$0 << 1 | 0 >>> 31) & -2 | $carry_0_lcssa$0;
  return (tempRet0 = $_0$1, $_0$0) | 0;
}
// =======================================================================
// EMSCRIPTEN_END_FUNCS
// TODO: strip out parts of this we do not need
//======= begin closure i64 code =======
// Copyright 2009 The Closure Library Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/**
 * @fileoverview Defines a Long class for representing a 64-bit two's-complement
 * integer value, which faithfully simulates the behavior of a Java "long". This
 * implementation is derived from LongLib in GWT.
 *
 */
var i64Math = (function() { // Emscripten wrapper
  var goog = { math: {} };
  /**
   * Constructs a 64-bit two's-complement integer, given its low and high 32-bit
   * values as *signed* integers.  See the from* functions below for more
   * convenient ways of constructing Longs.
   *
   * The internal representation of a long is the two given signed, 32-bit values.
   * We use 32-bit pieces because these are the size of integers on which
   * Javascript performs bit-operations.  For operations like addition and
   * multiplication, we split each number into 16-bit pieces, which can easily be
   * multiplied within Javascript's floating-point representation without overflow
   * or change in sign.
   *
   * In the algorithms below, we frequently reduce the negative case to the
   * positive case by negating the input(s) and then post-processing the result.
   * Note that we must ALWAYS check specially whether those values are MIN_VALUE
   * (-2^63) because -MIN_VALUE == MIN_VALUE (since 2^63 cannot be represented as
   * a positive number, it overflows back into a negative).  Not handling this
   * case would often result in infinite recursion.
   *
   * @param {number} low  The low (signed) 32 bits of the long.
   * @param {number} high  The high (signed) 32 bits of the long.
   * @constructor
   */
  goog.math.Long = function(low, high) {
    /**
     * @type {number}
     * @private
     */
    this.low_ = low | 0;  // force into 32 signed bits.
    /**
     * @type {number}
     * @private
     */
    this.high_ = high | 0;  // force into 32 signed bits.
  };
  // NOTE: Common constant values ZERO, ONE, NEG_ONE, etc. are defined below the
  // from* methods on which they depend.
  /**
   * A cache of the Long representations of small integer values.
   * @type {!Object}
   * @private
   */
  goog.math.Long.IntCache_ = {};
  /**
   * Returns a Long representing the given (32-bit) integer value.
   * @param {number} value The 32-bit integer in question.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromInt = function(value) {
    if (-128 <= value && value < 128) {
      var cachedObj = goog.math.Long.IntCache_[value];
      if (cachedObj) {
        return cachedObj;
      }
    }
    var obj = new goog.math.Long(value | 0, value < 0 ? -1 : 0);
    if (-128 <= value && value < 128) {
      goog.math.Long.IntCache_[value] = obj;
    }
    return obj;
  };
  /**
   * Returns a Long representing the given value, provided that it is a finite
   * number.  Otherwise, zero is returned.
   * @param {number} value The number in question.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromNumber = function(value) {
    if (isNaN(value) || !isFinite(value)) {
      return goog.math.Long.ZERO;
    } else if (value <= -goog.math.Long.TWO_PWR_63_DBL_) {
      return goog.math.Long.MIN_VALUE;
    } else if (value + 1 >= goog.math.Long.TWO_PWR_63_DBL_) {
      return goog.math.Long.MAX_VALUE;
    } else if (value < 0) {
      return goog.math.Long.fromNumber(-value).negate();
    } else {
      return new goog.math.Long(
          (value % goog.math.Long.TWO_PWR_32_DBL_) | 0,
          (value / goog.math.Long.TWO_PWR_32_DBL_) | 0);
    }
  };
  /**
   * Returns a Long representing the 64-bit integer that comes by concatenating
   * the given high and low bits.  Each is assumed to use 32 bits.
   * @param {number} lowBits The low 32-bits.
   * @param {number} highBits The high 32-bits.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromBits = function(lowBits, highBits) {
    return new goog.math.Long(lowBits, highBits);
  };
  /**
   * Returns a Long representation of the given string, written using the given
   * radix.
   * @param {string} str The textual representation of the Long.
   * @param {number=} opt_radix The radix in which the text is written.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromString = function(str, opt_radix) {
    if (str.length == 0) {
      throw Error('number format error: empty string');
    }
    var radix = opt_radix || 10;
    if (radix < 2 || 36 < radix) {
      throw Error('radix out of range: ' + radix);
    }
    if (str.charAt(0) == '-') {
      return goog.math.Long.fromString(str.substring(1), radix).negate();
    } else if (str.indexOf('-') >= 0) {
      throw Error('number format error: interior "-" character: ' + str);
    }
    // Do several (8) digits each time through the loop, so as to
    // minimize the calls to the very expensive emulated div.
    var radixToPower = goog.math.Long.fromNumber(Math.pow(radix, 8));
    var result = goog.math.Long.ZERO;
    for (var i = 0; i < str.length; i += 8) {
      var size = Math.min(8, str.length - i);
      var value = parseInt(str.substring(i, i + size), radix);
      if (size < 8) {
        var power = goog.math.Long.fromNumber(Math.pow(radix, size));
        result = result.multiply(power).add(goog.math.Long.fromNumber(value));
      } else {
        result = result.multiply(radixToPower);
        result = result.add(goog.math.Long.fromNumber(value));
      }
    }
    return result;
  };
  // NOTE: the compiler should inline these constant values below and then remove
  // these variables, so there should be no runtime penalty for these.
  /**
   * Number used repeated below in calculations.  This must appear before the
   * first call to any from* function below.
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_16_DBL_ = 1 << 16;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_24_DBL_ = 1 << 24;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_32_DBL_ =
      goog.math.Long.TWO_PWR_16_DBL_ * goog.math.Long.TWO_PWR_16_DBL_;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_31_DBL_ =
      goog.math.Long.TWO_PWR_32_DBL_ / 2;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_48_DBL_ =
      goog.math.Long.TWO_PWR_32_DBL_ * goog.math.Long.TWO_PWR_16_DBL_;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_64_DBL_ =
      goog.math.Long.TWO_PWR_32_DBL_ * goog.math.Long.TWO_PWR_32_DBL_;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_63_DBL_ =
      goog.math.Long.TWO_PWR_64_DBL_ / 2;
  /** @type {!goog.math.Long} */
  goog.math.Long.ZERO = goog.math.Long.fromInt(0);
  /** @type {!goog.math.Long} */
  goog.math.Long.ONE = goog.math.Long.fromInt(1);
  /** @type {!goog.math.Long} */
  goog.math.Long.NEG_ONE = goog.math.Long.fromInt(-1);
  /** @type {!goog.math.Long} */
  goog.math.Long.MAX_VALUE =
      goog.math.Long.fromBits(0xFFFFFFFF | 0, 0x7FFFFFFF | 0);
  /** @type {!goog.math.Long} */
  goog.math.Long.MIN_VALUE = goog.math.Long.fromBits(0, 0x80000000 | 0);
  /**
   * @type {!goog.math.Long}
   * @private
   */
  goog.math.Long.TWO_PWR_24_ = goog.math.Long.fromInt(1 << 24);
  /** @return {number} The value, assuming it is a 32-bit integer. */
  goog.math.Long.prototype.toInt = function() {
    return this.low_;
  };
  /** @return {number} The closest floating-point representation to this value. */
  goog.math.Long.prototype.toNumber = function() {
    return this.high_ * goog.math.Long.TWO_PWR_32_DBL_ +
           this.getLowBitsUnsigned();
  };
  /**
   * @param {number=} opt_radix The radix in which the text should be written.
   * @return {string} The textual representation of this value.
   */
  goog.math.Long.prototype.toString = function(opt_radix) {
    var radix = opt_radix || 10;
    if (radix < 2 || 36 < radix) {
      throw Error('radix out of range: ' + radix);
    }
    if (this.isZero()) {
      return '0';
    }
    if (this.isNegative()) {
      if (this.equals(goog.math.Long.MIN_VALUE)) {
        // We need to change the Long value before it can be negated, so we remove
        // the bottom-most digit in this base and then recurse to do the rest.
        var radixLong = goog.math.Long.fromNumber(radix);
        var div = this.div(radixLong);
        var rem = div.multiply(radixLong).subtract(this);
        return div.toString(radix) + rem.toInt().toString(radix);
      } else {
        return '-' + this.negate().toString(radix);
      }
    }
    // Do several (6) digits each time through the loop, so as to
    // minimize the calls to the very expensive emulated div.
    var radixToPower = goog.math.Long.fromNumber(Math.pow(radix, 6));
    var rem = this;
    var result = '';
    while (true) {
      var remDiv = rem.div(radixToPower);
      var intval = rem.subtract(remDiv.multiply(radixToPower)).toInt();
      var digits = intval.toString(radix);
      rem = remDiv;
      if (rem.isZero()) {
        return digits + result;
      } else {
        while (digits.length < 6) {
          digits = '0' + digits;
        }
        result = '' + digits + result;
      }
    }
  };
  /** @return {number} The high 32-bits as a signed value. */
  goog.math.Long.prototype.getHighBits = function() {
    return this.high_;
  };
  /** @return {number} The low 32-bits as a signed value. */
  goog.math.Long.prototype.getLowBits = function() {
    return this.low_;
  };
  /** @return {number} The low 32-bits as an unsigned value. */
  goog.math.Long.prototype.getLowBitsUnsigned = function() {
    return (this.low_ >= 0) ?
        this.low_ : goog.math.Long.TWO_PWR_32_DBL_ + this.low_;
  };
  /**
   * @return {number} Returns the number of bits needed to represent the absolute
   *     value of this Long.
   */
  goog.math.Long.prototype.getNumBitsAbs = function() {
    if (this.isNegative()) {
      if (this.equals(goog.math.Long.MIN_VALUE)) {
        return 64;
      } else {
        return this.negate().getNumBitsAbs();
      }
    } else {
      var val = this.high_ != 0 ? this.high_ : this.low_;
      for (var bit = 31; bit > 0; bit--) {
        if ((val & (1 << bit)) != 0) {
          break;
        }
      }
      return this.high_ != 0 ? bit + 33 : bit + 1;
    }
  };
  /** @return {boolean} Whether this value is zero. */
  goog.math.Long.prototype.isZero = function() {
    return this.high_ == 0 && this.low_ == 0;
  };
  /** @return {boolean} Whether this value is negative. */
  goog.math.Long.prototype.isNegative = function() {
    return this.high_ < 0;
  };
  /** @return {boolean} Whether this value is odd. */
  goog.math.Long.prototype.isOdd = function() {
    return (this.low_ & 1) == 1;
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long equals the other.
   */
  goog.math.Long.prototype.equals = function(other) {
    return (this.high_ == other.high_) && (this.low_ == other.low_);
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long does not equal the other.
   */
  goog.math.Long.prototype.notEquals = function(other) {
    return (this.high_ != other.high_) || (this.low_ != other.low_);
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is less than the other.
   */
  goog.math.Long.prototype.lessThan = function(other) {
    return this.compare(other) < 0;
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is less than or equal to the other.
   */
  goog.math.Long.prototype.lessThanOrEqual = function(other) {
    return this.compare(other) <= 0;
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is greater than the other.
   */
  goog.math.Long.prototype.greaterThan = function(other) {
    return this.compare(other) > 0;
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is greater than or equal to the other.
   */
  goog.math.Long.prototype.greaterThanOrEqual = function(other) {
    return this.compare(other) >= 0;
  };
  /**
   * Compares this Long with the given one.
   * @param {goog.math.Long} other Long to compare against.
   * @return {number} 0 if they are the same, 1 if the this is greater, and -1
   *     if the given one is greater.
   */
  goog.math.Long.prototype.compare = function(other) {
    if (this.equals(other)) {
      return 0;
    }
    var thisNeg = this.isNegative();
    var otherNeg = other.isNegative();
    if (thisNeg && !otherNeg) {
      return -1;
    }
    if (!thisNeg && otherNeg) {
      return 1;
    }
    // at this point, the signs are the same, so subtraction will not overflow
    if (this.subtract(other).isNegative()) {
      return -1;
    } else {
      return 1;
    }
  };
  /** @return {!goog.math.Long} The negation of this value. */
  goog.math.Long.prototype.negate = function() {
    if (this.equals(goog.math.Long.MIN_VALUE)) {
      return goog.math.Long.MIN_VALUE;
    } else {
      return this.not().add(goog.math.Long.ONE);
    }
  };
  /**
   * Returns the sum of this and the given Long.
   * @param {goog.math.Long} other Long to add to this one.
   * @return {!goog.math.Long} The sum of this and the given Long.
   */
  goog.math.Long.prototype.add = function(other) {
    // Divide each number into 4 chunks of 16 bits, and then sum the chunks.
    var a48 = this.high_ >>> 16;
    var a32 = this.high_ & 0xFFFF;
    var a16 = this.low_ >>> 16;
    var a00 = this.low_ & 0xFFFF;
    var b48 = other.high_ >>> 16;
    var b32 = other.high_ & 0xFFFF;
    var b16 = other.low_ >>> 16;
    var b00 = other.low_ & 0xFFFF;
    var c48 = 0, c32 = 0, c16 = 0, c00 = 0;
    c00 += a00 + b00;
    c16 += c00 >>> 16;
    c00 &= 0xFFFF;
    c16 += a16 + b16;
    c32 += c16 >>> 16;
    c16 &= 0xFFFF;
    c32 += a32 + b32;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c48 += a48 + b48;
    c48 &= 0xFFFF;
    return goog.math.Long.fromBits((c16 << 16) | c00, (c48 << 16) | c32);
  };
  /**
   * Returns the difference of this and the given Long.
   * @param {goog.math.Long} other Long to subtract from this.
   * @return {!goog.math.Long} The difference of this and the given Long.
   */
  goog.math.Long.prototype.subtract = function(other) {
    return this.add(other.negate());
  };
  /**
   * Returns the product of this and the given long.
   * @param {goog.math.Long} other Long to multiply with this.
   * @return {!goog.math.Long} The product of this and the other.
   */
  goog.math.Long.prototype.multiply = function(other) {
    if (this.isZero()) {
      return goog.math.Long.ZERO;
    } else if (other.isZero()) {
      return goog.math.Long.ZERO;
    }
    if (this.equals(goog.math.Long.MIN_VALUE)) {
      return other.isOdd() ? goog.math.Long.MIN_VALUE : goog.math.Long.ZERO;
    } else if (other.equals(goog.math.Long.MIN_VALUE)) {
      return this.isOdd() ? goog.math.Long.MIN_VALUE : goog.math.Long.ZERO;
    }
    if (this.isNegative()) {
      if (other.isNegative()) {
        return this.negate().multiply(other.negate());
      } else {
        return this.negate().multiply(other).negate();
      }
    } else if (other.isNegative()) {
      return this.multiply(other.negate()).negate();
    }
    // If both longs are small, use float multiplication
    if (this.lessThan(goog.math.Long.TWO_PWR_24_) &&
        other.lessThan(goog.math.Long.TWO_PWR_24_)) {
      return goog.math.Long.fromNumber(this.toNumber() * other.toNumber());
    }
    // Divide each long into 4 chunks of 16 bits, and then add up 4x4 products.
    // We can skip products that would overflow.
    var a48 = this.high_ >>> 16;
    var a32 = this.high_ & 0xFFFF;
    var a16 = this.low_ >>> 16;
    var a00 = this.low_ & 0xFFFF;
    var b48 = other.high_ >>> 16;
    var b32 = other.high_ & 0xFFFF;
    var b16 = other.low_ >>> 16;
    var b00 = other.low_ & 0xFFFF;
    var c48 = 0, c32 = 0, c16 = 0, c00 = 0;
    c00 += a00 * b00;
    c16 += c00 >>> 16;
    c00 &= 0xFFFF;
    c16 += a16 * b00;
    c32 += c16 >>> 16;
    c16 &= 0xFFFF;
    c16 += a00 * b16;
    c32 += c16 >>> 16;
    c16 &= 0xFFFF;
    c32 += a32 * b00;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c32 += a16 * b16;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c32 += a00 * b32;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c48 += a48 * b00 + a32 * b16 + a16 * b32 + a00 * b48;
    c48 &= 0xFFFF;
    return goog.math.Long.fromBits((c16 << 16) | c00, (c48 << 16) | c32);
  };
  /**
   * Returns this Long divided by the given one.
   * @param {goog.math.Long} other Long by which to divide.
   * @return {!goog.math.Long} This Long divided by the given one.
   */
  goog.math.Long.prototype.div = function(other) {
    if (other.isZero()) {
      throw Error('division by zero');
    } else if (this.isZero()) {
      return goog.math.Long.ZERO;
    }
    if (this.equals(goog.math.Long.MIN_VALUE)) {
      if (other.equals(goog.math.Long.ONE) ||
          other.equals(goog.math.Long.NEG_ONE)) {
        return goog.math.Long.MIN_VALUE;  // recall that -MIN_VALUE == MIN_VALUE
      } else if (other.equals(goog.math.Long.MIN_VALUE)) {
        return goog.math.Long.ONE;
      } else {
        // At this point, we have |other| >= 2, so |this/other| < |MIN_VALUE|.
        var halfThis = this.shiftRight(1);
        var approx = halfThis.div(other).shiftLeft(1);
        if (approx.equals(goog.math.Long.ZERO)) {
          return other.isNegative() ? goog.math.Long.ONE : goog.math.Long.NEG_ONE;
        } else {
          var rem = this.subtract(other.multiply(approx));
          var result = approx.add(rem.div(other));
          return result;
        }
      }
    } else if (other.equals(goog.math.Long.MIN_VALUE)) {
      return goog.math.Long.ZERO;
    }
    if (this.isNegative()) {
      if (other.isNegative()) {
        return this.negate().div(other.negate());
      } else {
        return this.negate().div(other).negate();
      }
    } else if (other.isNegative()) {
      return this.div(other.negate()).negate();
    }
    // Repeat the following until the remainder is less than other:  find a
    // floating-point that approximates remainder / other *from below*, add this
    // into the result, and subtract it from the remainder.  It is critical that
    // the approximate value is less than or equal to the real value so that the
    // remainder never becomes negative.
    var res = goog.math.Long.ZERO;
    var rem = this;
    while (rem.greaterThanOrEqual(other)) {
      // Approximate the result of division. This may be a little greater or
      // smaller than the actual value.
      var approx = Math.max(1, Math.floor(rem.toNumber() / other.toNumber()));
      // We will tweak the approximate result by changing it in the 48-th digit or
      // the smallest non-fractional digit, whichever is larger.
      var log2 = Math.ceil(Math.log(approx) / Math.LN2);
      var delta = (log2 <= 48) ? 1 : Math.pow(2, log2 - 48);
      // Decrease the approximation until it is smaller than the remainder.  Note
      // that if it is too large, the product overflows and is negative.
      var approxRes = goog.math.Long.fromNumber(approx);
      var approxRem = approxRes.multiply(other);
      while (approxRem.isNegative() || approxRem.greaterThan(rem)) {
        approx -= delta;
        approxRes = goog.math.Long.fromNumber(approx);
        approxRem = approxRes.multiply(other);
      }
      // We know the answer can't be zero... and actually, zero would cause
      // infinite recursion since we would make no progress.
      if (approxRes.isZero()) {
        approxRes = goog.math.Long.ONE;
      }
      res = res.add(approxRes);
      rem = rem.subtract(approxRem);
    }
    return res;
  };
  /**
   * Returns this Long modulo the given one.
   * @param {goog.math.Long} other Long by which to mod.
   * @return {!goog.math.Long} This Long modulo the given one.
   */
  goog.math.Long.prototype.modulo = function(other) {
    return this.subtract(this.div(other).multiply(other));
  };
  /** @return {!goog.math.Long} The bitwise-NOT of this value. */
  goog.math.Long.prototype.not = function() {
    return goog.math.Long.fromBits(~this.low_, ~this.high_);
  };
  /**
   * Returns the bitwise-AND of this Long and the given one.
   * @param {goog.math.Long} other The Long with which to AND.
   * @return {!goog.math.Long} The bitwise-AND of this and the other.
   */
  goog.math.Long.prototype.and = function(other) {
    return goog.math.Long.fromBits(this.low_ & other.low_,
                                   this.high_ & other.high_);
  };
  /**
   * Returns the bitwise-OR of this Long and the given one.
   * @param {goog.math.Long} other The Long with which to OR.
   * @return {!goog.math.Long} The bitwise-OR of this and the other.
   */
  goog.math.Long.prototype.or = function(other) {
    return goog.math.Long.fromBits(this.low_ | other.low_,
                                   this.high_ | other.high_);
  };
  /**
   * Returns the bitwise-XOR of this Long and the given one.
   * @param {goog.math.Long} other The Long with which to XOR.
   * @return {!goog.math.Long} The bitwise-XOR of this and the other.
   */
  goog.math.Long.prototype.xor = function(other) {
    return goog.math.Long.fromBits(this.low_ ^ other.low_,
                                   this.high_ ^ other.high_);
  };
  /**
   * Returns this Long with bits shifted to the left by the given amount.
   * @param {number} numBits The number of bits by which to shift.
   * @return {!goog.math.Long} This shifted to the left by the given amount.
   */
  goog.math.Long.prototype.shiftLeft = function(numBits) {
    numBits &= 63;
    if (numBits == 0) {
      return this;
    } else {
      var low = this.low_;
      if (numBits < 32) {
        var high = this.high_;
        return goog.math.Long.fromBits(
            low << numBits,
            (high << numBits) | (low >>> (32 - numBits)));
      } else {
        return goog.math.Long.fromBits(0, low << (numBits - 32));
      }
    }
  };
  /**
   * Returns this Long with bits shifted to the right by the given amount.
   * @param {number} numBits The number of bits by which to shift.
   * @return {!goog.math.Long} This shifted to the right by the given amount.
   */
  goog.math.Long.prototype.shiftRight = function(numBits) {
    numBits &= 63;
    if (numBits == 0) {
      return this;
    } else {
      var high = this.high_;
      if (numBits < 32) {
        var low = this.low_;
        return goog.math.Long.fromBits(
            (low >>> numBits) | (high << (32 - numBits)),
            high >> numBits);
      } else {
        return goog.math.Long.fromBits(
            high >> (numBits - 32),
            high >= 0 ? 0 : -1);
      }
    }
  };
  /**
   * Returns this Long with bits shifted to the right by the given amount, with
   * the new top bits matching the current sign bit.
   * @param {number} numBits The number of bits by which to shift.
   * @return {!goog.math.Long} This shifted to the right by the given amount, with
   *     zeros placed into the new leading bits.
   */
  goog.math.Long.prototype.shiftRightUnsigned = function(numBits) {
    numBits &= 63;
    if (numBits == 0) {
      return this;
    } else {
      var high = this.high_;
      if (numBits < 32) {
        var low = this.low_;
        return goog.math.Long.fromBits(
            (low >>> numBits) | (high << (32 - numBits)),
            high >>> numBits);
      } else if (numBits == 32) {
        return goog.math.Long.fromBits(high, 0);
      } else {
        return goog.math.Long.fromBits(high >>> (numBits - 32), 0);
      }
    }
  };
  //======= begin jsbn =======
  var navigator = { appName: 'Modern Browser' }; // polyfill a little
  // Copyright (c) 2005  Tom Wu
  // All Rights Reserved.
  // http://www-cs-students.stanford.edu/~tjw/jsbn/
  /*
   * Copyright (c) 2003-2005  Tom Wu
   * All Rights Reserved.
   *
   * Permission is hereby granted, free of charge, to any person obtaining
   * a copy of this software and associated documentation files (the
   * "Software"), to deal in the Software without restriction, including
   * without limitation the rights to use, copy, modify, merge, publish,
   * distribute, sublicense, and/or sell copies of the Software, and to
   * permit persons to whom the Software is furnished to do so, subject to
   * the following conditions:
   *
   * The above copyright notice and this permission notice shall be
   * included in all copies or substantial portions of the Software.
   *
   * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
   * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
   * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
   *
   * IN NO EVENT SHALL TOM WU BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
   * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
   * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF
   * THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT
   * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
   *
   * In addition, the following condition applies:
   *
   * All redistributions must retain an intact copy of this copyright notice
   * and disclaimer.
   */
  // Basic JavaScript BN library - subset useful for RSA encryption.
  // Bits per digit
  var dbits;
  // JavaScript engine analysis
  var canary = 0xdeadbeefcafe;
  var j_lm = ((canary&0xffffff)==0xefcafe);
  // (public) Constructor
  function BigInteger(a,b,c) {
    if(a != null)
      if("number" == typeof a) this.fromNumber(a,b,c);
      else if(b == null && "string" != typeof a) this.fromString(a,256);
      else this.fromString(a,b);
  }
  // return new, unset BigInteger
  function nbi() { return new BigInteger(null); }
  // am: Compute w_j += (x*this_i), propagate carries,
  // c is initial carry, returns final carry.
  // c < 3*dvalue, x < 2*dvalue, this_i < dvalue
  // We need to select the fastest one that works in this environment.
  // am1: use a single mult and divide to get the high bits,
  // max digit bits should be 26 because
  // max internal value = 2*dvalue^2-2*dvalue (< 2^53)
  function am1(i,x,w,j,c,n) {
    while(--n >= 0) {
      var v = x*this[i++]+w[j]+c;
      c = Math.floor(v/0x4000000);
      w[j++] = v&0x3ffffff;
    }
    return c;
  }
  // am2 avoids a big mult-and-extract completely.
  // Max digit bits should be <= 30 because we do bitwise ops
  // on values up to 2*hdvalue^2-hdvalue-1 (< 2^31)
  function am2(i,x,w,j,c,n) {
    var xl = x&0x7fff, xh = x>>15;
    while(--n >= 0) {
      var l = this[i]&0x7fff;
      var h = this[i++]>>15;
      var m = xh*l+h*xl;
      l = xl*l+((m&0x7fff)<<15)+w[j]+(c&0x3fffffff);
      c = (l>>>30)+(m>>>15)+xh*h+(c>>>30);
      w[j++] = l&0x3fffffff;
    }
    return c;
  }
  // Alternately, set max digit bits to 28 since some
  // browsers slow down when dealing with 32-bit numbers.
  function am3(i,x,w,j,c,n) {
    var xl = x&0x3fff, xh = x>>14;
    while(--n >= 0) {
      var l = this[i]&0x3fff;
      var h = this[i++]>>14;
      var m = xh*l+h*xl;
      l = xl*l+((m&0x3fff)<<14)+w[j]+c;
      c = (l>>28)+(m>>14)+xh*h;
      w[j++] = l&0xfffffff;
    }
    return c;
  }
  if(j_lm && (navigator.appName == "Microsoft Internet Explorer")) {
    BigInteger.prototype.am = am2;
    dbits = 30;
  }
  else if(j_lm && (navigator.appName != "Netscape")) {
    BigInteger.prototype.am = am1;
    dbits = 26;
  }
  else { // Mozilla/Netscape seems to prefer am3
    BigInteger.prototype.am = am3;
    dbits = 28;
  }
  BigInteger.prototype.DB = dbits;
  BigInteger.prototype.DM = ((1<<dbits)-1);
  BigInteger.prototype.DV = (1<<dbits);
  var BI_FP = 52;
  BigInteger.prototype.FV = Math.pow(2,BI_FP);
  BigInteger.prototype.F1 = BI_FP-dbits;
  BigInteger.prototype.F2 = 2*dbits-BI_FP;
  // Digit conversions
  var BI_RM = "0123456789abcdefghijklmnopqrstuvwxyz";
  var BI_RC = new Array();
  var rr,vv;
  rr = "0".charCodeAt(0);
  for(vv = 0; vv <= 9; ++vv) BI_RC[rr++] = vv;
  rr = "a".charCodeAt(0);
  for(vv = 10; vv < 36; ++vv) BI_RC[rr++] = vv;
  rr = "A".charCodeAt(0);
  for(vv = 10; vv < 36; ++vv) BI_RC[rr++] = vv;
  function int2char(n) { return BI_RM.charAt(n); }
  function intAt(s,i) {
    var c = BI_RC[s.charCodeAt(i)];
    return (c==null)?-1:c;
  }
  // (protected) copy this to r
  function bnpCopyTo(r) {
    for(var i = this.t-1; i >= 0; --i) r[i] = this[i];
    r.t = this.t;
    r.s = this.s;
  }
  // (protected) set from integer value x, -DV <= x < DV
  function bnpFromInt(x) {
    this.t = 1;
    this.s = (x<0)?-1:0;
    if(x > 0) this[0] = x;
    else if(x < -1) this[0] = x+DV;
    else this.t = 0;
  }
  // return bigint initialized to value
  function nbv(i) { var r = nbi(); r.fromInt(i); return r; }
  // (protected) set from string and radix
  function bnpFromString(s,b) {
    var k;
    if(b == 16) k = 4;
    else if(b == 8) k = 3;
    else if(b == 256) k = 8; // byte array
    else if(b == 2) k = 1;
    else if(b == 32) k = 5;
    else if(b == 4) k = 2;
    else { this.fromRadix(s,b); return; }
    this.t = 0;
    this.s = 0;
    var i = s.length, mi = false, sh = 0;
    while(--i >= 0) {
      var x = (k==8)?s[i]&0xff:intAt(s,i);
      if(x < 0) {
        if(s.charAt(i) == "-") mi = true;
        continue;
      }
      mi = false;
      if(sh == 0)
        this[this.t++] = x;
      else if(sh+k > this.DB) {
        this[this.t-1] |= (x&((1<<(this.DB-sh))-1))<<sh;
        this[this.t++] = (x>>(this.DB-sh));
      }
      else
        this[this.t-1] |= x<<sh;
      sh += k;
      if(sh >= this.DB) sh -= this.DB;
    }
    if(k == 8 && (s[0]&0x80) != 0) {
      this.s = -1;
      if(sh > 0) this[this.t-1] |= ((1<<(this.DB-sh))-1)<<sh;
    }
    this.clamp();
    if(mi) BigInteger.ZERO.subTo(this,this);
  }
  // (protected) clamp off excess high words
  function bnpClamp() {
    var c = this.s&this.DM;
    while(this.t > 0 && this[this.t-1] == c) --this.t;
  }
  // (public) return string representation in given radix
  function bnToString(b) {
    if(this.s < 0) return "-"+this.negate().toString(b);
    var k;
    if(b == 16) k = 4;
    else if(b == 8) k = 3;
    else if(b == 2) k = 1;
    else if(b == 32) k = 5;
    else if(b == 4) k = 2;
    else return this.toRadix(b);
    var km = (1<<k)-1, d, m = false, r = "", i = this.t;
    var p = this.DB-(i*this.DB)%k;
    if(i-- > 0) {
      if(p < this.DB && (d = this[i]>>p) > 0) { m = true; r = int2char(d); }
      while(i >= 0) {
        if(p < k) {
          d = (this[i]&((1<<p)-1))<<(k-p);
          d |= this[--i]>>(p+=this.DB-k);
        }
        else {
          d = (this[i]>>(p-=k))&km;
          if(p <= 0) { p += this.DB; --i; }
        }
        if(d > 0) m = true;
        if(m) r += int2char(d);
      }
    }
    return m?r:"0";
  }
  // (public) -this
  function bnNegate() { var r = nbi(); BigInteger.ZERO.subTo(this,r); return r; }
  // (public) |this|
  function bnAbs() { return (this.s<0)?this.negate():this; }
  // (public) return + if this > a, - if this < a, 0 if equal
  function bnCompareTo(a) {
    var r = this.s-a.s;
    if(r != 0) return r;
    var i = this.t;
    r = i-a.t;
    if(r != 0) return (this.s<0)?-r:r;
    while(--i >= 0) if((r=this[i]-a[i]) != 0) return r;
    return 0;
  }
  // returns bit length of the integer x
  function nbits(x) {
    var r = 1, t;
    if((t=x>>>16) != 0) { x = t; r += 16; }
    if((t=x>>8) != 0) { x = t; r += 8; }
    if((t=x>>4) != 0) { x = t; r += 4; }
    if((t=x>>2) != 0) { x = t; r += 2; }
    if((t=x>>1) != 0) { x = t; r += 1; }
    return r;
  }
  // (public) return the number of bits in "this"
  function bnBitLength() {
    if(this.t <= 0) return 0;
    return this.DB*(this.t-1)+nbits(this[this.t-1]^(this.s&this.DM));
  }
  // (protected) r = this << n*DB
  function bnpDLShiftTo(n,r) {
    var i;
    for(i = this.t-1; i >= 0; --i) r[i+n] = this[i];
    for(i = n-1; i >= 0; --i) r[i] = 0;
    r.t = this.t+n;
    r.s = this.s;
  }
  // (protected) r = this >> n*DB
  function bnpDRShiftTo(n,r) {
    for(var i = n; i < this.t; ++i) r[i-n] = this[i];
    r.t = Math.max(this.t-n,0);
    r.s = this.s;
  }
  // (protected) r = this << n
  function bnpLShiftTo(n,r) {
    var bs = n%this.DB;
    var cbs = this.DB-bs;
    var bm = (1<<cbs)-1;
    var ds = Math.floor(n/this.DB), c = (this.s<<bs)&this.DM, i;
    for(i = this.t-1; i >= 0; --i) {
      r[i+ds+1] = (this[i]>>cbs)|c;
      c = (this[i]&bm)<<bs;
    }
    for(i = ds-1; i >= 0; --i) r[i] = 0;
    r[ds] = c;
    r.t = this.t+ds+1;
    r.s = this.s;
    r.clamp();
  }
  // (protected) r = this >> n
  function bnpRShiftTo(n,r) {
    r.s = this.s;
    var ds = Math.floor(n/this.DB);
    if(ds >= this.t) { r.t = 0; return; }
    var bs = n%this.DB;
    var cbs = this.DB-bs;
    var bm = (1<<bs)-1;
    r[0] = this[ds]>>bs;
    for(var i = ds+1; i < this.t; ++i) {
      r[i-ds-1] |= (this[i]&bm)<<cbs;
      r[i-ds] = this[i]>>bs;
    }
    if(bs > 0) r[this.t-ds-1] |= (this.s&bm)<<cbs;
    r.t = this.t-ds;
    r.clamp();
  }
  // (protected) r = this - a
  function bnpSubTo(a,r) {
    var i = 0, c = 0, m = Math.min(a.t,this.t);
    while(i < m) {
      c += this[i]-a[i];
      r[i++] = c&this.DM;
      c >>= this.DB;
    }
    if(a.t < this.t) {
      c -= a.s;
      while(i < this.t) {
        c += this[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c += this.s;
    }
    else {
      c += this.s;
      while(i < a.t) {
        c -= a[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c -= a.s;
    }
    r.s = (c<0)?-1:0;
    if(c < -1) r[i++] = this.DV+c;
    else if(c > 0) r[i++] = c;
    r.t = i;
    r.clamp();
  }
  // (protected) r = this * a, r != this,a (HAC 14.12)
  // "this" should be the larger one if appropriate.
  function bnpMultiplyTo(a,r) {
    var x = this.abs(), y = a.abs();
    var i = x.t;
    r.t = i+y.t;
    while(--i >= 0) r[i] = 0;
    for(i = 0; i < y.t; ++i) r[i+x.t] = x.am(0,y[i],r,i,0,x.t);
    r.s = 0;
    r.clamp();
    if(this.s != a.s) BigInteger.ZERO.subTo(r,r);
  }
  // (protected) r = this^2, r != this (HAC 14.16)
  function bnpSquareTo(r) {
    var x = this.abs();
    var i = r.t = 2*x.t;
    while(--i >= 0) r[i] = 0;
    for(i = 0; i < x.t-1; ++i) {
      var c = x.am(i,x[i],r,2*i,0,1);
      if((r[i+x.t]+=x.am(i+1,2*x[i],r,2*i+1,c,x.t-i-1)) >= x.DV) {
        r[i+x.t] -= x.DV;
        r[i+x.t+1] = 1;
      }
    }
    if(r.t > 0) r[r.t-1] += x.am(i,x[i],r,2*i,0,1);
    r.s = 0;
    r.clamp();
  }
  // (protected) divide this by m, quotient and remainder to q, r (HAC 14.20)
  // r != q, this != m.  q or r may be null.
  function bnpDivRemTo(m,q,r) {
    var pm = m.abs();
    if(pm.t <= 0) return;
    var pt = this.abs();
    if(pt.t < pm.t) {
      if(q != null) q.fromInt(0);
      if(r != null) this.copyTo(r);
      return;
    }
    if(r == null) r = nbi();
    var y = nbi(), ts = this.s, ms = m.s;
    var nsh = this.DB-nbits(pm[pm.t-1]);	// normalize modulus
    if(nsh > 0) { pm.lShiftTo(nsh,y); pt.lShiftTo(nsh,r); }
    else { pm.copyTo(y); pt.copyTo(r); }
    var ys = y.t;
    var y0 = y[ys-1];
    if(y0 == 0) return;
    var yt = y0*(1<<this.F1)+((ys>1)?y[ys-2]>>this.F2:0);
    var d1 = this.FV/yt, d2 = (1<<this.F1)/yt, e = 1<<this.F2;
    var i = r.t, j = i-ys, t = (q==null)?nbi():q;
    y.dlShiftTo(j,t);
    if(r.compareTo(t) >= 0) {
      r[r.t++] = 1;
      r.subTo(t,r);
    }
    BigInteger.ONE.dlShiftTo(ys,t);
    t.subTo(y,y);	// "negative" y so we can replace sub with am later
    while(y.t < ys) y[y.t++] = 0;
    while(--j >= 0) {
      // Estimate quotient digit
      var qd = (r[--i]==y0)?this.DM:Math.floor(r[i]*d1+(r[i-1]+e)*d2);
      if((r[i]+=y.am(0,qd,r,j,0,ys)) < qd) {	// Try it out
        y.dlShiftTo(j,t);
        r.subTo(t,r);
        while(r[i] < --qd) r.subTo(t,r);
      }
    }
    if(q != null) {
      r.drShiftTo(ys,q);
      if(ts != ms) BigInteger.ZERO.subTo(q,q);
    }
    r.t = ys;
    r.clamp();
    if(nsh > 0) r.rShiftTo(nsh,r);	// Denormalize remainder
    if(ts < 0) BigInteger.ZERO.subTo(r,r);
  }
  // (public) this mod a
  function bnMod(a) {
    var r = nbi();
    this.abs().divRemTo(a,null,r);
    if(this.s < 0 && r.compareTo(BigInteger.ZERO) > 0) a.subTo(r,r);
    return r;
  }
  // Modular reduction using "classic" algorithm
  function Classic(m) { this.m = m; }
  function cConvert(x) {
    if(x.s < 0 || x.compareTo(this.m) >= 0) return x.mod(this.m);
    else return x;
  }
  function cRevert(x) { return x; }
  function cReduce(x) { x.divRemTo(this.m,null,x); }
  function cMulTo(x,y,r) { x.multiplyTo(y,r); this.reduce(r); }
  function cSqrTo(x,r) { x.squareTo(r); this.reduce(r); }
  Classic.prototype.convert = cConvert;
  Classic.prototype.revert = cRevert;
  Classic.prototype.reduce = cReduce;
  Classic.prototype.mulTo = cMulTo;
  Classic.prototype.sqrTo = cSqrTo;
  // (protected) return "-1/this % 2^DB"; useful for Mont. reduction
  // justification:
  //         xy == 1 (mod m)
  //         xy =  1+km
  //   xy(2-xy) = (1+km)(1-km)
  // x[y(2-xy)] = 1-k^2m^2
  // x[y(2-xy)] == 1 (mod m^2)
  // if y is 1/x mod m, then y(2-xy) is 1/x mod m^2
  // should reduce x and y(2-xy) by m^2 at each step to keep size bounded.
  // JS multiply "overflows" differently from C/C++, so care is needed here.
  function bnpInvDigit() {
    if(this.t < 1) return 0;
    var x = this[0];
    if((x&1) == 0) return 0;
    var y = x&3;		// y == 1/x mod 2^2
    y = (y*(2-(x&0xf)*y))&0xf;	// y == 1/x mod 2^4
    y = (y*(2-(x&0xff)*y))&0xff;	// y == 1/x mod 2^8
    y = (y*(2-(((x&0xffff)*y)&0xffff)))&0xffff;	// y == 1/x mod 2^16
    // last step - calculate inverse mod DV directly;
    // assumes 16 < DB <= 32 and assumes ability to handle 48-bit ints
    y = (y*(2-x*y%this.DV))%this.DV;		// y == 1/x mod 2^dbits
    // we really want the negative inverse, and -DV < y < DV
    return (y>0)?this.DV-y:-y;
  }
  // Montgomery reduction
  function Montgomery(m) {
    this.m = m;
    this.mp = m.invDigit();
    this.mpl = this.mp&0x7fff;
    this.mph = this.mp>>15;
    this.um = (1<<(m.DB-15))-1;
    this.mt2 = 2*m.t;
  }
  // xR mod m
  function montConvert(x) {
    var r = nbi();
    x.abs().dlShiftTo(this.m.t,r);
    r.divRemTo(this.m,null,r);
    if(x.s < 0 && r.compareTo(BigInteger.ZERO) > 0) this.m.subTo(r,r);
    return r;
  }
  // x/R mod m
  function montRevert(x) {
    var r = nbi();
    x.copyTo(r);
    this.reduce(r);
    return r;
  }
  // x = x/R mod m (HAC 14.32)
  function montReduce(x) {
    while(x.t <= this.mt2)	// pad x so am has enough room later
      x[x.t++] = 0;
    for(var i = 0; i < this.m.t; ++i) {
      // faster way of calculating u0 = x[i]*mp mod DV
      var j = x[i]&0x7fff;
      var u0 = (j*this.mpl+(((j*this.mph+(x[i]>>15)*this.mpl)&this.um)<<15))&x.DM;
      // use am to combine the multiply-shift-add into one call
      j = i+this.m.t;
      x[j] += this.m.am(0,u0,x,i,0,this.m.t);
      // propagate carry
      while(x[j] >= x.DV) { x[j] -= x.DV; x[++j]++; }
    }
    x.clamp();
    x.drShiftTo(this.m.t,x);
    if(x.compareTo(this.m) >= 0) x.subTo(this.m,x);
  }
  // r = "x^2/R mod m"; x != r
  function montSqrTo(x,r) { x.squareTo(r); this.reduce(r); }
  // r = "xy/R mod m"; x,y != r
  function montMulTo(x,y,r) { x.multiplyTo(y,r); this.reduce(r); }
  Montgomery.prototype.convert = montConvert;
  Montgomery.prototype.revert = montRevert;
  Montgomery.prototype.reduce = montReduce;
  Montgomery.prototype.mulTo = montMulTo;
  Montgomery.prototype.sqrTo = montSqrTo;
  // (protected) true iff this is even
  function bnpIsEven() { return ((this.t>0)?(this[0]&1):this.s) == 0; }
  // (protected) this^e, e < 2^32, doing sqr and mul with "r" (HAC 14.79)
  function bnpExp(e,z) {
    if(e > 0xffffffff || e < 1) return BigInteger.ONE;
    var r = nbi(), r2 = nbi(), g = z.convert(this), i = nbits(e)-1;
    g.copyTo(r);
    while(--i >= 0) {
      z.sqrTo(r,r2);
      if((e&(1<<i)) > 0) z.mulTo(r2,g,r);
      else { var t = r; r = r2; r2 = t; }
    }
    return z.revert(r);
  }
  // (public) this^e % m, 0 <= e < 2^32
  function bnModPowInt(e,m) {
    var z;
    if(e < 256 || m.isEven()) z = new Classic(m); else z = new Montgomery(m);
    return this.exp(e,z);
  }
  // protected
  BigInteger.prototype.copyTo = bnpCopyTo;
  BigInteger.prototype.fromInt = bnpFromInt;
  BigInteger.prototype.fromString = bnpFromString;
  BigInteger.prototype.clamp = bnpClamp;
  BigInteger.prototype.dlShiftTo = bnpDLShiftTo;
  BigInteger.prototype.drShiftTo = bnpDRShiftTo;
  BigInteger.prototype.lShiftTo = bnpLShiftTo;
  BigInteger.prototype.rShiftTo = bnpRShiftTo;
  BigInteger.prototype.subTo = bnpSubTo;
  BigInteger.prototype.multiplyTo = bnpMultiplyTo;
  BigInteger.prototype.squareTo = bnpSquareTo;
  BigInteger.prototype.divRemTo = bnpDivRemTo;
  BigInteger.prototype.invDigit = bnpInvDigit;
  BigInteger.prototype.isEven = bnpIsEven;
  BigInteger.prototype.exp = bnpExp;
  // public
  BigInteger.prototype.toString = bnToString;
  BigInteger.prototype.negate = bnNegate;
  BigInteger.prototype.abs = bnAbs;
  BigInteger.prototype.compareTo = bnCompareTo;
  BigInteger.prototype.bitLength = bnBitLength;
  BigInteger.prototype.mod = bnMod;
  BigInteger.prototype.modPowInt = bnModPowInt;
  // "constants"
  BigInteger.ZERO = nbv(0);
  BigInteger.ONE = nbv(1);
  // jsbn2 stuff
  // (protected) convert from radix string
  function bnpFromRadix(s,b) {
    this.fromInt(0);
    if(b == null) b = 10;
    var cs = this.chunkSize(b);
    var d = Math.pow(b,cs), mi = false, j = 0, w = 0;
    for(var i = 0; i < s.length; ++i) {
      var x = intAt(s,i);
      if(x < 0) {
        if(s.charAt(i) == "-" && this.signum() == 0) mi = true;
        continue;
      }
      w = b*w+x;
      if(++j >= cs) {
        this.dMultiply(d);
        this.dAddOffset(w,0);
        j = 0;
        w = 0;
      }
    }
    if(j > 0) {
      this.dMultiply(Math.pow(b,j));
      this.dAddOffset(w,0);
    }
    if(mi) BigInteger.ZERO.subTo(this,this);
  }
  // (protected) return x s.t. r^x < DV
  function bnpChunkSize(r) { return Math.floor(Math.LN2*this.DB/Math.log(r)); }
  // (public) 0 if this == 0, 1 if this > 0
  function bnSigNum() {
    if(this.s < 0) return -1;
    else if(this.t <= 0 || (this.t == 1 && this[0] <= 0)) return 0;
    else return 1;
  }
  // (protected) this *= n, this >= 0, 1 < n < DV
  function bnpDMultiply(n) {
    this[this.t] = this.am(0,n-1,this,0,0,this.t);
    ++this.t;
    this.clamp();
  }
  // (protected) this += n << w words, this >= 0
  function bnpDAddOffset(n,w) {
    if(n == 0) return;
    while(this.t <= w) this[this.t++] = 0;
    this[w] += n;
    while(this[w] >= this.DV) {
      this[w] -= this.DV;
      if(++w >= this.t) this[this.t++] = 0;
      ++this[w];
    }
  }
  // (protected) convert to radix string
  function bnpToRadix(b) {
    if(b == null) b = 10;
    if(this.signum() == 0 || b < 2 || b > 36) return "0";
    var cs = this.chunkSize(b);
    var a = Math.pow(b,cs);
    var d = nbv(a), y = nbi(), z = nbi(), r = "";
    this.divRemTo(d,y,z);
    while(y.signum() > 0) {
      r = (a+z.intValue()).toString(b).substr(1) + r;
      y.divRemTo(d,y,z);
    }
    return z.intValue().toString(b) + r;
  }
  // (public) return value as integer
  function bnIntValue() {
    if(this.s < 0) {
      if(this.t == 1) return this[0]-this.DV;
      else if(this.t == 0) return -1;
    }
    else if(this.t == 1) return this[0];
    else if(this.t == 0) return 0;
    // assumes 16 < DB < 32
    return ((this[1]&((1<<(32-this.DB))-1))<<this.DB)|this[0];
  }
  // (protected) r = this + a
  function bnpAddTo(a,r) {
    var i = 0, c = 0, m = Math.min(a.t,this.t);
    while(i < m) {
      c += this[i]+a[i];
      r[i++] = c&this.DM;
      c >>= this.DB;
    }
    if(a.t < this.t) {
      c += a.s;
      while(i < this.t) {
        c += this[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c += this.s;
    }
    else {
      c += this.s;
      while(i < a.t) {
        c += a[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c += a.s;
    }
    r.s = (c<0)?-1:0;
    if(c > 0) r[i++] = c;
    else if(c < -1) r[i++] = this.DV+c;
    r.t = i;
    r.clamp();
  }
  BigInteger.prototype.fromRadix = bnpFromRadix;
  BigInteger.prototype.chunkSize = bnpChunkSize;
  BigInteger.prototype.signum = bnSigNum;
  BigInteger.prototype.dMultiply = bnpDMultiply;
  BigInteger.prototype.dAddOffset = bnpDAddOffset;
  BigInteger.prototype.toRadix = bnpToRadix;
  BigInteger.prototype.intValue = bnIntValue;
  BigInteger.prototype.addTo = bnpAddTo;
  //======= end jsbn =======
  // Emscripten wrapper
  var Wrapper = {
    abs: function(l, h) {
      var x = new goog.math.Long(l, h);
      var ret;
      if (x.isNegative()) {
        ret = x.negate();
      } else {
        ret = x;
      }
      HEAP32[tempDoublePtr>>2] = ret.low_;
      HEAP32[tempDoublePtr+4>>2] = ret.high_;
    },
    ensureTemps: function() {
      if (Wrapper.ensuredTemps) return;
      Wrapper.ensuredTemps = true;
      Wrapper.two32 = new BigInteger();
      Wrapper.two32.fromString('4294967296', 10);
      Wrapper.two64 = new BigInteger();
      Wrapper.two64.fromString('18446744073709551616', 10);
      Wrapper.temp1 = new BigInteger();
      Wrapper.temp2 = new BigInteger();
    },
    lh2bignum: function(l, h) {
      var a = new BigInteger();
      a.fromString(h.toString(), 10);
      var b = new BigInteger();
      a.multiplyTo(Wrapper.two32, b);
      var c = new BigInteger();
      c.fromString(l.toString(), 10);
      var d = new BigInteger();
      c.addTo(b, d);
      return d;
    },
    stringify: function(l, h, unsigned) {
      var ret = new goog.math.Long(l, h).toString();
      if (unsigned && ret[0] == '-') {
        // unsign slowly using jsbn bignums
        Wrapper.ensureTemps();
        var bignum = new BigInteger();
        bignum.fromString(ret, 10);
        ret = new BigInteger();
        Wrapper.two64.addTo(bignum, ret);
        ret = ret.toString(10);
      }
      return ret;
    },
    fromString: function(str, base, min, max, unsigned) {
      Wrapper.ensureTemps();
      var bignum = new BigInteger();
      bignum.fromString(str, base);
      var bigmin = new BigInteger();
      bigmin.fromString(min, 10);
      var bigmax = new BigInteger();
      bigmax.fromString(max, 10);
      if (unsigned && bignum.compareTo(BigInteger.ZERO) < 0) {
        var temp = new BigInteger();
        bignum.addTo(Wrapper.two64, temp);
        bignum = temp;
      }
      var error = false;
      if (bignum.compareTo(bigmin) < 0) {
        bignum = bigmin;
        error = true;
      } else if (bignum.compareTo(bigmax) > 0) {
        bignum = bigmax;
        error = true;
      }
      var ret = goog.math.Long.fromString(bignum.toString()); // min-max checks should have clamped this to a range goog.math.Long can handle well
      HEAP32[tempDoublePtr>>2] = ret.low_;
      HEAP32[tempDoublePtr+4>>2] = ret.high_;
      if (error) throw 'range error';
    }
  };
  return Wrapper;
})();
//======= end closure i64 code =======
// === Auto-generated postamble setup entry stuff ===
Module['callMain'] = function callMain(args) {
  assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on __ATMAIN__)');
  assert(!Module['preRun'] || Module['preRun'].length == 0, 'cannot call main when preRun functions remain to be called');
  args = args || [];
  ensureInitRuntime();
  var argc = args.length+1;
  function pad() {
    for (var i = 0; i < 4-1; i++) {
      argv.push(0);
    }
  }
  var argv = [allocate(intArrayFromString("/bin/this.program"), 'i8', ALLOC_NORMAL) ];
  pad();
  for (var i = 0; i < argc-1; i = i + 1) {
    argv.push(allocate(intArrayFromString(args[i]), 'i8', ALLOC_NORMAL));
    pad();
  }
  argv.push(0);
  argv = allocate(argv, 'i32', ALLOC_NORMAL);
  var ret;
  var initialStackTop = STACKTOP;
  try {
    ret = Module['_main'](argc, argv, 0);
  }
  catch(e) {
    if (e.name == 'ExitStatus') {
      return e.status;
    } else if (e == 'SimulateInfiniteLoop') {
      Module['noExitRuntime'] = true;
    } else {
      throw e;
    }
  } finally {
    STACKTOP = initialStackTop;
  }
  return ret;
}
function run(args) {
  args = args || Module['arguments'];
  if (runDependencies > 0) {
    Module.printErr('run() called, but dependencies remain, so not running');
    return 0;
  }
  if (Module['preRun']) {
    if (typeof Module['preRun'] == 'function') Module['preRun'] = [Module['preRun']];
    var toRun = Module['preRun'];
    Module['preRun'] = [];
    for (var i = toRun.length-1; i >= 0; i--) {
      toRun[i]();
    }
    if (runDependencies > 0) {
      // a preRun added a dependency, run will be called later
      return 0;
    }
  }
  function doRun() {
    ensureInitRuntime();
    preMain();
    var ret = 0;
    calledRun = true;
    if (Module['_main'] && shouldRunNow) {
      ret = Module['callMain'](args);
      if (!Module['noExitRuntime']) {
        exitRuntime();
      }
    }
    if (Module['postRun']) {
      if (typeof Module['postRun'] == 'function') Module['postRun'] = [Module['postRun']];
      while (Module['postRun'].length > 0) {
        Module['postRun'].pop()();
      }
    }
    return ret;
  }
  if (Module['setStatus']) {
    Module['setStatus']('Running...');
    setTimeout(function() {
      setTimeout(function() {
        Module['setStatus']('');
      }, 1);
      if (!ABORT) doRun();
    }, 1);
    return 0;
  } else {
    return doRun();
  }
}
Module['run'] = Module.run = run;
// {{PRE_RUN_ADDITIONS}}
if (Module['preInit']) {
  if (typeof Module['preInit'] == 'function') Module['preInit'] = [Module['preInit']];
  while (Module['preInit'].length > 0) {
    Module['preInit'].pop()();
  }
}
// shouldRunNow refers to calling main(), not run().
var shouldRunNow = true;
if (Module['noInitialRun']) {
  shouldRunNow = false;
}
run();
// {{POST_RUN_ADDITIONS}}
  // {{MODULE_ADDITIONS}}
