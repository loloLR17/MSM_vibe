# Incidents d’outillage, sans rejeu des essais

- Inventaire Windows lecture seule : caractère CP850 non décodable UTF-8 par Python ; inventaire refait avec sortie UTF-8 explicite. Aucun COM ouvert ni accès cible dans ces commandes.
- Contrôle documentaire VDDIO2 : pdftotext absent (127), puis module pymupdf absent (1). Aucune installation ni protection changée. Netlist officielle C05 et UM2861 ST consultés directement.
- Observation GDB du boot passif : nom de membre `tr2_irq_observer.init` inexistant ; erreur GDB conservée. Correction vers `after_uart_enable`, nouvelle lecture des seuls états, sans nouveau flash/reset/armement ni émission.

Ces erreurs ne sont pas des échecs UART physiques et ne sont pas supprimées des preuves.

- `powershell -File host_probe.ps1` refusé (`UnauthorizedAccess`, scripts désactivés) avant exécution du script/ouvertureCOM. Journal brut et invocation conservés, pas de nouvelle tentative sous autre forme, pas de Bypass/déblocage/changement de politique. Les cinq scopes sont Undefined (politique effective par défaut Restricted).
- Les maîtres historiques inspectés commencent par B0 ou séquences métier inadaptées ; aucun exécuté. Client natif séparé ClickHost construit avec csc.NET Framework existant, contrôle hors bus réussi, application non signée acceptée par Windows avec Smart App Control=1 et zéro3033/3077 corrélé dans les fenêtres relues. Il utilise le canal d'exécution normal des applications, sans charger le script PowerShell refusé. Pas de certificat créé ni confiance ajoutée. Ce succès ne constitue pas une méthode durable de signature/déploiement.

- Contrôle Git : les journaux bruts Windows comportent CRLF et espaces de présentation CubeProgrammer/PowerShell ; `git diff --check` global les signale. Ils restent byteexact pour préserver les preuves. Le contrôle de format est appliqué séparément au code/documents/JSON, hors `.log`/`.jsonl` bruts ; aucune normalisation ni modification de ces sorties pour obtenir un succès.
