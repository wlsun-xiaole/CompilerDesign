#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             std::vector<DeclNode> *p_decls,
                          					 std::vector<AstNode*> *p_stats)
    										: AstNode{line, col}, 
    										decls(*p_decls), stats(*p_stats)
    										{}

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // decl
    for (auto &decl : decls) {
    	decl.accept(p_visitor);
    }

    // statements
    for (auto &stat : stats) {
    	CompoundStatementNode* csp = dynamic_cast<CompoundStatementNode *>(stat);
    	if (csp != NULL) {
    		csp->accept(p_visitor);
    	}

        PrintNode* pp = dynamic_cast<PrintNode*>(stat);
        if (pp != NULL) {
            pp->accept(p_visitor);
        }

        AssignmentNode *ap = dynamic_cast<AssignmentNode*>(stat);
        if (ap != NULL) {
            ap->accept(p_visitor);
        }

        ReadNode *rp = dynamic_cast<ReadNode*>(stat);
        if (rp != NULL) {
            rp->accept(p_visitor);
        }

        IfNode *ip = dynamic_cast<IfNode*>(stat);
        if (ip != NULL) {
            ip->accept(p_visitor);
        }

        FunctionInvocationNode *fip = dynamic_cast<FunctionInvocationNode *>(stat);
        if (fip != NULL) {
            fip->accept(p_visitor);
        }

        WhileNode *wp = dynamic_cast<WhileNode*>(stat);
        if (wp != NULL) {
            wp->accept(p_visitor);
        }

        ForNode *fp = dynamic_cast<ForNode*>(stat);
        if (fp != NULL) {
            fp->accept(p_visitor);
        }

        ReturnNode *rtp = dynamic_cast<ReturnNode*>(stat);
        if (rtp != NULL) {
            rtp->accept(p_visitor);
        }
    }

}
void CompoundStatementNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}

