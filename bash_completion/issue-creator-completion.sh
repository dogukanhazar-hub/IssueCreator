_issue_creator_completions() {
    local cur prev opts commands
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    commands="register update-key create update list help github"
    opts="--apikey --owner --repo --title --description --number --state --labels"

    # Ana komut seviyesindeysek
    if [[ ${COMP_CWORD} == 1 ]]; then
        COMPREPLY=( $(compgen -W "${commands}" -- "${cur}") )
        return 0
    fi

    # github alt komutuysa özel muamele yap
    if [[ "${COMP_WORDS[1]}" == "github" ]]; then
        local github_subcommands="register update-key create update list"
        local subcommand="${COMP_WORDS[2]}"
        
        if [[ ${COMP_CWORD} == 2 ]]; then
            COMPREPLY=( $(compgen -W "${github_subcommands}" -- "${cur}") )
            return 0
        fi

        case "$subcommand" in
            register|update-key)
                COMPREPLY=( $(compgen -W "--apikey" -- "${cur}") )
                ;;
            create)
                COMPREPLY=( $(compgen -W "--owner --repo --title --description --labels" -- "${cur}") )
                ;;
            update)
                COMPREPLY=( $(compgen -W "--owner --repo --number --title --description --state --labels" -- "${cur}") )
                ;;
            list)
                COMPREPLY=( $(compgen -W "--owner --repo" -- "${cur}") )
                ;;
        esac

        return 0
    fi

    # Diğer (gitee) komutlar
    local command="${COMP_WORDS[1]}"
    case "$command" in
        register|update-key)
            COMPREPLY=( $(compgen -W "--apikey" -- "${cur}") )
            ;;
        create)
            COMPREPLY=( $(compgen -W "--owner --repo --title --description --labels" -- "${cur}") )
            ;;
        update)
            COMPREPLY=( $(compgen -W "--owner --repo --number --title --description --state --labels" -- "${cur}") )
            ;;
        list)
            COMPREPLY=( $(compgen -W "--owner --repo" -- "${cur}") )
            ;;
    esac
}
complete -F _issue_creator_completions issue-creator

