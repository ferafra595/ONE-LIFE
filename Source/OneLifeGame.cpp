#include "OneLifeGame.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

UOneLifeSimulationComponent::UOneLifeSimulationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UOneLifeSimulationComponent::BeginPlay()
{
    Super::BeginPlay();

    Needs.FindOrAdd(EOneLifeNeed::Hunger) = 90.f;
    Needs.FindOrAdd(EOneLifeNeed::Thirst) = 90.f;
    Needs.FindOrAdd(EOneLifeNeed::Energy) = 90.f;
    Needs.FindOrAdd(EOneLifeNeed::Hygiene) = 90.f;
    Needs.FindOrAdd(EOneLifeNeed::Social) = 75.f;
    Needs.FindOrAdd(EOneLifeNeed::Stress) = 15.f;
    Needs.FindOrAdd(EOneLifeNeed::Bladder) = 90.f;
}

float UOneLifeSimulationComponent::GetNeed(EOneLifeNeed Need) const
{
    if (const float* V = Needs.Find(Need)) return *V;
    return 0.f;
}

void UOneLifeSimulationComponent::ModifyNeed(EOneLifeNeed Need, float Delta)
{
    float& V = Needs.FindOrAdd(Need);
    V = FMath::Clamp(V + Delta, 0.f, 100.f);
    OnNeedChanged.Broadcast(Need, V);
}

void UOneLifeSimulationComponent::AdvanceNeeds(float H)
{
    ModifyNeed(EOneLifeNeed::Hunger, -3.0f * H);
    ModifyNeed(EOneLifeNeed::Thirst, -4.2f * H);
    ModifyNeed(EOneLifeNeed::Energy, -2.0f * H);
    ModifyNeed(EOneLifeNeed::Hygiene, -1.1f * H);
    ModifyNeed(EOneLifeNeed::Social, -0.7f * H);
    ModifyNeed(EOneLifeNeed::Bladder, -4.5f * H);
    ModifyNeed(EOneLifeNeed::Stress, 0.25f * H);
}

void UOneLifeSimulationComponent::AddMemory(const FLifeMemory& Memory)
{
    Memories.Add(Memory);
}

void UOneLifeWorldSubsystem::AdvanceMinutes(int32 Minutes)
{
    CurrentDateTime += FTimespan::FromMinutes(Minutes);
    OnTimeChanged.Broadcast(CurrentDateTime);
}

float UOneLifeWorldSubsystem::GetHour() const
{
    return CurrentDateTime.GetHour() + CurrentDateTime.GetMinute()/60.f;
}

AOneLifeCharacter::AOneLifeCharacter()
{
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 360.f;
    CameraBoom->SocketOffset = FVector(0,45,65);
    CameraBoom->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

    Simulation = CreateDefaultSubobject<UOneLifeSimulationComponent>(TEXT("Simulation"));
}

void AOneLifeCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (auto* Input = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (MappingContext) Input->AddMappingContext(MappingContext,0);
            }
        }
    }
}

void AOneLifeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction) EIC->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AOneLifeCharacter::Move);
        if (LookAction) EIC->BindAction(LookAction,ETriggerEvent::Triggered,this,&AOneLifeCharacter::Look);
        if (JumpAction) EIC->BindAction(JumpAction,ETriggerEvent::Started,this,&ACharacter::Jump);

        if (SprintAction)
        {
            EIC->BindAction(SprintAction,ETriggerEvent::Started,this,&AOneLifeCharacter::SprintStart);
            EIC->BindAction(SprintAction,ETriggerEvent::Completed,this,&AOneLifeCharacter::SprintStop);
        }

        if (InteractAction) EIC->BindAction(InteractAction,ETriggerEvent::Started,this,&AOneLifeCharacter::Interact);
    }
}

void AOneLifeCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) return;

    FVector2D Axis = Value.Get<FVector2D>();
    FRotator Yaw(0,Controller->GetControlRotation().Yaw,0);

    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X),Axis.Y);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y),Axis.X);
}

void AOneLifeCharacter::Look(const FInputActionValue& Value)
{
    FVector2D Axis = Value.Get<FVector2D>();
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y);
}

void AOneLifeCharacter::SprintStart()
{
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AOneLifeCharacter::SprintStop()
{
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AOneLifeCharacter::Interact()
{
    if (!Camera) return;

    FVector Start = Camera->GetComponentLocation();
    FVector End = Start + Camera->GetForwardVector()*InteractionDistance;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(OneLifeInteraction),false,this);

    if (GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Params))
    {
        AActor* Target = Hit.GetActor();

        if (Target && Target->GetClass()->ImplementsInterface(UOneLifeInteractable::StaticClass()))
        {
            if (IOneLifeInteractable::Execute_CanInteract(Target,this))
                IOneLifeInteractable::Execute_Interact(Target,this);
        }
    }
}

AOneLifeGameMode::AOneLifeGameMode()
{
    DefaultPawnClass = AOneLifeCharacter::StaticClass();
}
