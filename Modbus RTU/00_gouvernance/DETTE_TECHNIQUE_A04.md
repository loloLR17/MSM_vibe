# MSM TR2 — Registre des constats et dettes A-04

**Source :** audit Codex `TR2-20261013-A04-AUDIT-FIRMWARE`, rapport `Echanges_Codex/DERNIER_RAPPORT.md` (état publié au 13/10/2026), preuves `Echanges_Codex/preuves/TR2-20261013-A04/`. **Statut :** registre ouvert, constats non corrigés et non requalifiés. La classification reprend le rapport Codex ; la décision de priorisation est ChatGPT/utilisateur. Toute clôture exige une preuve et un commit, pas seulement une déclaration.

| ID | Gravité A-04 | Constat / dette | Traitement prévu | Statut |
|---|---|---|---|---|
| A04-01 | Bloquant avant qualification physique M016 | Profil CMake générique non cumulatif ; harness susceptible d'écrire sur microSD | **Correction 1 :** profil cumulatif explicite, garde-fous contre mauvais binaire et écriture SD ; tests de matrice CMake/ELF | OUVERT |
| A04-02 | Bloquant avant qualification physique M016 | Scan/commit FRAM synchrones sur chemin Modbus B3/B5 ; trou historique ~141 ms, budget FIFO ~17 ms depuis vide | **Correction 2 :** accès/projections bornés ou politique d'indisponibilité pendant capture, sans affaiblir persistance/recovery ; tests temporels et physiques ciblés | OUVERT |
| A04-03 | Bloquant avant qualification physique M016 | FIFO M015-V diagnostic non raccordée aux fenêtres métier V1/configuration/publication | **M016 :** propriétaire unique, fenêtres validées, générations et invalidations, intégration V1 sans stockage implicite | OUVERT |
| A04-04 | Important | Tests hôte ne couvrent pas tout le glue STM32 et sa concurrence | **A-05 :** matrice et tests ciblés à définir avant corrections | OUVERT |
| A04-05 | Important | Monotonie timestamp ne démontre pas cadence ni absence de perte globale | Définir tolérances/cadence et scénarios gap/wrap avant revendication stricte | OUVERT |
| A04-06 | Important | Marge RAM statique ~179,5 Kio, instrumentation et buffers volumineux | Budget RAM/stack/heap avant FFT/DSP, réutiliser buffers | OUVERT |
| A04-07 | Important | Préparation FRAM monolithique au démarrage après UART | Tester trafic au boot et borner/exposer correctement READY avant qualification système | OUVERT |
| A04-08 | Important pour SD, non bloquant M016 sans SD | Backend SDMMC synchrone ; risques de stalls/partition | Mission dédiée avant activation SD ; maintenir SD désactivée pour M016 sans stockage | OUVERT |
| A04-09 | Facultatif | Génération d'exemple ST et instrumentation historique non liée | Différer toute simplification sans bénéfice mesuré | DIFFÉRÉ |

## Ordre décidé et critères de sortie

1. **A-05 (audit des tests, sans correction)** : évaluer la couverture et définir des tests pertinents, reproductibles et proportionnés pour A04-01, A04-02, A04-03, sans oublier A04-04 à A04-08. Ne pas confondre host, ARM et essai physique.
2. **Correction 1 — A04-01** : verrouiller la sélection du build sûr et les chemins d'écriture SD. Préserver l'image cumulative de référence ou justifier toute divergence.
3. **Correction 2 — A04-02** : empêcher les chemins FRAM/Modbus de compromettre les deadlines FIFO ; conserver intégrité, durabilité, recovery, B5 et B0/B1.
4. **M016 — A04-03** : raccorder acquisition FIFO et publication vibratoire V1 sous configuration autoritative, avec budget mémoire et politique d'intégrité des timestamps.
5. **SD — A04-08** : qualification séparée avant activation ; A04-09 seulement si valeur démontrée.

**Traçabilité :** conserver ID A04-xx dans chaque contrat/rapport/commit pertinent ; inscrire après correction SHA, tests, preuves, limitations, statut `CORRIGÉ / À QUALIFIER / QUALIFIÉ` et date. Les états `OUVERT` ne changent pas implicitement lors d'un rapport `DONE`. Les preuves physiques M015-V demeurent bornées et A-01 (qualification indépendante/tag) reste distincte.
