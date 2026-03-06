/*
 *  bitvector.cppm – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  lam.bitvector is a c++ module
 */

export module lam.bitvector;

export import :core;
export import :boolean_ops;
export import :algorithms;
export import :config;

export namespace lam
{
using bitvec::bitvector;
using bitvec::bitvector_allocator_c;
} // namespace lam
