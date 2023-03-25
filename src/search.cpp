#include "../include/attacks.hpp"
#include "../include/move_generation.hpp"
#include "../include/uci_commands.hpp"
#include "../include/board.hpp"
#include "../include/evaluation.hpp"

#define ANSPIRATION_WINDOW 50
#define FULL_DEPTH_MOVES_NUM 3
#define REDUCT_LIMIT 2
#define R 2

#define EVAL_MARGIN 120
#define RAZORING_BONUS1 125
#define RAZORING_BONUS2 175

static unsigned long nodes;
static int ply;
gameBoard* cur_board;

static int quiescence(int alpha, int beta)
{
    if ((nodes % 2047) == 0)
        cur_board->communicate();
    
    if (ply+1 > MAX_PLY)
        // evaluate position
        return evaluate_position(*cur_board);
    
    int evaluation = evaluate_position(*cur_board);
    nodes++;
    
    if (evaluation >= beta)
        return beta;

    if (evaluation > alpha)
        alpha = evaluation;

    _move_list generated_moves[1]; generate_moves(*cur_board, generated_moves, 0, ply);
    
    for (short move_number = 0; move_number < generated_moves->number_of_moves; move_number++)
    {
        copy_board_p(cur_board);

        // we only want captures
        if (!make_move(*cur_board, generated_moves->moves[move_number].move, capture_move)) continue;

        ply++;
        cur_board->add_to_history();

        int score = -quiescence(-beta, -alpha);

        ply--;
        cur_board->rep_index--;
        revert_board_p(cur_board);

        if (cur_board->stopped) return 0;
        
        if (score > alpha)
        {
            alpha = score;

            if (score >= beta)
                return beta;
        }
    }

    return alpha;
}

static int negamax(int alpha, int beta, short depth)
{
    if ((nodes % 2047) == 0)
        cur_board->communicate();
    
    int score;
    Move_Type b_move;
    
    bool pv_node = (beta-alpha) > 1;

    if (ply && cur_board->is_repetition())
        return 0;

    // read hash entry
    if (ply && !pv_node && (score = tt_search(cur_board->TT, cur_board->hash_position, b_move, ply, alpha, beta, depth)) != HASH_NONE)
        return score;
    
    // define hash flag
    hash_flag hflag = HASH_ALPHA;
    
    cur_board->pv_length[ply] = ply;
    
    if (depth == 0)
        return quiescence(alpha, beta);
    
    nodes++;

    // we reached too deep, evaluate position
    if (ply+1 > MAX_PLY)
        return evaluate_position(*cur_board);
    
    bool is_in_check = cur_board->is_king_in_check();
    if (is_in_check) depth++;

    int static_eval;
    bool st_null_move_pr = false;
    
    // static null move pruning
    if (depth < 3 && !pv_node && !is_in_check && abs(beta - 1) > NEG_INFINITY + 100)
    {
        int eval_margin = 120*depth;

        st_null_move_pr = true;
        static_eval = evaluate_position(*cur_board);
                
        if (static_eval - eval_margin >= beta)
            return static_eval - eval_margin;
    }

    // null move pruning
    if (depth > R && !is_in_check && ply)
    {
        make_null_move(cur_board, ply)

        // still testing reduction:|
        // * 4 + depth / 6 + min((score - beta) / 256, 3);
        // * 3 + depth / 5 + min(3, (score - beta) / 256);
        // * -R-1

        score = -negamax(-beta, -beta+1, depth-R-1);

        revert_null_move(cur_board, ply)

        if (score >= beta)
            return beta;
    }

    // razoring
    if (!pv_node && !is_in_check && depth < 4)
    {
        if (!st_null_move_pr) score = evaluate_position(*cur_board) + RAZORING_BONUS1;
        else score = static_eval + RAZORING_BONUS1;

        if (score < beta)
        {
            if (depth == 1)
            {
                int new_score = quiescence(alpha, beta);
                return (new_score > score) ? new_score : score;
            }
            
            score += RAZORING_BONUS2;
            
            if (score < beta && depth < 3)
            {
                int new_score = quiescence(alpha, beta);
                if (new_score < beta)
                    return (new_score > score) ? new_score : score;
            }
        }
	}

    // generate moves
    _move_list generated_moves[1]; generate_moves(*cur_board, generated_moves, b_move, ply);

    sint legal_moves = 0;  // the number of legal moves in the position
    int moves_searched = 0;

    for (short move_number = 0; move_number < generated_moves->number_of_moves; move_number++)
    {
        copy_board_p(cur_board);

        // illegal move, skip it and move to the next one
        if (!make_move(*cur_board, generated_moves->moves[move_number].move, all_moves)) continue;
        
        ply++;
        cur_board->add_to_history();

        legal_moves++;

        int score;

        if (moves_searched == 0)
            score = -negamax(-beta, -alpha, depth-1);
        else
        {
            // Late move reduction
            if
            (moves_searched > FULL_DEPTH_MOVES_NUM
             && depth > REDUCT_LIMIT  // Depth < 3
             && !is_in_check  // Moves while in check
             && !get_capture(generated_moves->moves[move_number].move) && !get_promoted_piece(generated_moves->moves[move_number].move)  // Tactical Moves
            )
                score = -negamax(-alpha-1, -alpha, depth-2);
            else
                score = alpha + 1;

            if (score > alpha)
            {
                score = -negamax(-alpha-1, -alpha, depth-1);
                if (score > alpha && score < beta)
                    score = -negamax(-beta, -alpha, depth-1);
            }
        }

        ply--;
        cur_board->rep_index--;

        revert_board_p(cur_board);

        // out of time, return
        if (cur_board->stopped) return 0;

        moves_searched++;

        if (score > alpha)
        {
            hflag = HASH_EXACT;

            // save best move
            b_move = generated_moves->moves[move_number].move;

            alpha = score;
            
            // update history move heuristic
            cur_board->update_history_move(generated_moves->moves[move_number].move, depth);

            // save the principal variation
            cur_board->update_PV(generated_moves->moves[move_number].move, ply);

            if (score >= beta)
            {
                // update killer move heuristic
                cur_board->update_killer_move(generated_moves->moves[move_number].move, ply);

                tt_insert(cur_board->TT, cur_board->hash_position, b_move, ply, beta, depth, HASH_BETA);
                return beta;
            }
        }
    }

    if (!legal_moves)  // king has no legal moves
    {
        if (is_in_check) return -MATE_VALUE+ply;  // checkmate
        return 0;  // stalemate
    }

    // insert position to the TT
    tt_insert(cur_board->TT, cur_board->hash_position, b_move, ply, alpha, depth, hflag);

    return alpha;
}

void best_move(gameBoard* board, short depth)
{
    // reset heuristics
    cur_board = board;
    for (sint killer_move = 0; killer_move < NUM_OF_KILLER_MOVES; killer_move++)
        for (sint j = 0; j < MAX_PLY; j++) cur_board->killer_moves[killer_move][j] = 0;
    for (sint piece = 0; piece < NUM_OF_PIECES; piece++)
    {
        cur_board->pv_length[piece] = 0;
        for (sint j = 0; j < SQUARES; j++) cur_board->history_moves[piece][j] = 0;
    }
    for (sint square = 0; square < SQUARES; square++)
        for (sint j = 0; j < SQUARES; j++) cur_board->pv_table[square][j] = 0;
    
    cur_board->score_pv = false;

    int alpha = NEG_INFINITY;
    int beta = INFINITY;

    cur_board->stopped = 0;  // start timer

    Move_Type cur_best_move;

    for (short cur_depth = 1, max_depth = depth+1; cur_depth < max_depth; cur_depth++)
    {
        if (cur_board->stopped) break;

        nodes = ply = 0;
        cur_board->follow_pv = true;

        int score = negamax(alpha, beta, cur_depth);

        if (score <= alpha || score >= beta)
        {
            alpha = NEG_INFINITY;
            beta = INFINITY; 
            continue;
        }

        alpha = score-ANSPIRATION_WINDOW;
        beta = score+ANSPIRATION_WINDOW;

        // print information about the search
        if (!cur_board->stopped)
        {
            if (score > -MATE_VALUE && score < -MATE_SCORE)
                cout << "info score mate " << -(score + MATE_VALUE)/2-1 << " depth " << cur_depth << " nodes " << nodes << " time " << get_time() - cur_board->starttime << " pv ";
            else if (score > MATE_SCORE && score < MATE_VALUE)
                cout << "info score mate " << (MATE_VALUE-score)/2+1 << " depth " << cur_depth << " nodes " << nodes << " time " << get_time() - cur_board->starttime << " pv ";
            else
                cout << "info score cp " << (int)score << " depth " << cur_depth << " nodes " << nodes << " time " << get_time() - cur_board->starttime << " pv ";

            cur_best_move = cur_board->pv_table[0][0]; 
            for (short count = 0; count < cur_board->pv_length[0]; count++)
            {
                print_move(cur_board->pv_table[0][count]);
                cout << " ";
            }
            cout << endl;
        }
        else break;
    }

    // print found move
    cout << "\nbestmove "; print_move(cur_best_move); cout << endl;
}
