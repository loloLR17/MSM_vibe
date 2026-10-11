---
mission_id: TR2-20261011-M013-A-TX-PREFLIGHT
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-A — Qualification preflight physique TX RS-485

## Etat acquis et objectif

Mission012 DONE pour le logiciel : 107/107 tests, cross-build STM32 TX OFF et ON, mais carte conserve BIN008, aucun flash ni TX physique. RX011 est prouve sur un seul essai HP. Le candidat TX ON de M012 est garde OFF par defaut et le binding STM32 production reste NOT_AVAILABLE. Ne pas confondre code hote valide et serveur Modbus operationnel sur la carte.

Objectif : lever, par preuves existantes et diagnostics non destructifs, les preconditions de securite TX (PG4 DE, /RE, RST, J2/J3, rail VIO, carte Click et configuration half-duplex), et qualifier ou preparer le plus petit essai de reponse physique Modbus V1. Ne pas faire un flash TX simplement parce que allow_flash est true.

## Execution

1. Lire integralement AGENTS.md, ETAT_COURANT_TR2.md, protocole V2, DERNIER_RAPPORT mission012, INTEGRATION_RTU_TX_012.md, documents de cablage et schemas de reference, documentation constructeur pertinente. Verifier HEAD reel main et sauvegarder les preuves.
2. Construire une matrice factuelle des conditions TX : etat prouve, source exacte, lacune, risque et moyen minimal de lever. Ne pas redemander les mesures 3,31V, pont J2 soude, SB28/SB29 et RX011 deja etablis ; ces preuves ne suffisent toutefois pas a affirmer la tension VIO Click ou l'absence de conflit /RE-RST.
3. Verifier le runtime STM32 de production, binding NOT_AVAILABLE, identite et adresse Modbus, configuration et readiness ; identifier les raccordements indispensables pour repondre sur la carte sans inventer de configuration de production ni effacer FRAM. Implementer si possible une tranche de diagnostic explicitement isolee et sure, avec tests de non-regression, permettant une reponse FC03 systeme sans activer les commandes/stockages non provisionnes. Toute deviation de la specification V1 doit etre exclue. Le choix architectural doit etre justifie et rester minimal.
4. Compiler et tester les modifications necessaires. Autorisation de flash et debug autonome selon AGENTS.md si toutes les conditions de surete et la procedure qualifiee sont reunies. Avant flash : sauvegarde image existante, verification cible, empreintes et lecture apres flash. Ne pas effectuer d'emission TX sur un bus dont les conditions electriques et de direction restent incertaines. Ne pas utiliser un ELF different du BIN effectivement flashe pour interpreter les compteurs.
5. Si les preuves documentaires ne suffisent pas, arreter l'activation TX et produire une demande d'intervention physique unique, precise, courte et motivee (point, methode, valeur attendue, consequence). Ne pas effectuer de manipulation physique ni exiger un nouvel essai RX. Aucun essai HP automatique ; preparer la procedure unique de requete/reponse pour la sous-mission suivante si le preflight est valide.

## Livrables

Publier un document M013-A avec matrice preflight, analyse binding, modifications et validations, conditions resolues/non resolues, verdict GO/NO-GO TX et prochain essai minimal. Mettre a jour ETAT_COURANT_TR2.md. Publier DERNIER_RAPPORT.md avec mission_id, HEAD, SHA, tests, etat flash/debug/TX reel et preuves. Commit/push normaux sur main et verification distante. Preserver freezes V1, travaux utilisateur et stashes. Interdits : mass erase, Option Bytes, force push, activation TX sans preuve de surete.
